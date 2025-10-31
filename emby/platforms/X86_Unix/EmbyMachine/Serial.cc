#include <EmbyMachine/Serial.hh>
#include <EmbyDebug/assert.hh>
#include <EmbyThreading/Thread.hh>
#include <EmbyThreading/ScopedMutex.hh>
#include <EmbyThreading/Worker.hh>
#include <EmbyLibs/StringUtils.hh>
#include <EmbyLog/Log.hh>

using namespace EmbyLibs;


EmbyLog_MODULE_LOG_CLASS("tty");

// Global list of opened Serial instances for the process.
// May be used by higher-level code to enumerate or manage serial ports.
static std::vector<EmbyMachine::Serial *> g_serials;

extern "C"
{
#include <sys/ioctl.h>
#include <fcntl.h>
#include "pthread.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#include <errno.h>
#include <pty.h>
#include <string.h>
}


namespace EmbyMachine
{

    // Create a pseudo-terminal (PTY) pair and expose the slave side via a symlink
    // at `device_name`. Returns the master fd on success or -1 on failure.
    // This is used to emulate a serial device file that other processes can open.
    int create_pty_device(const char* device_name) {
        int master_fd;
        char *slave_name;

        // Open a new PTY master
        master_fd = posix_openpt(O_RDWR | O_NOCTTY);
        if (master_fd < 0) {
            log_debug("PTY creation failed: %s", strerror(errno));
            return -1;
        }

        // Grant access and unlock the slave pseudoterminal
        if (grantpt(master_fd) < 0 || unlockpt(master_fd) < 0) {
            log_debug("PTY grant/unlock failed: %s", strerror(errno));
            close(master_fd);
            return -1;
        }

        // Obtain the slave device path for the created master
        slave_name = ptsname(master_fd);
        if (!slave_name) {
            log_debug("PTY ptsname failed: %s", strerror(errno));
            close(master_fd);
            return -1;
        }

        // Remove any existing file at device_name and create a symlink pointing to
        // the actual slave PTY device. This lets other parts of the system open
        // `device_name` as if it were a traditional serial device.
        unlink(device_name);
        if (symlink(slave_name, device_name) < 0) {
            log_debug("PTY symlink failed: %s", strerror(errno));
            close(master_fd);
            return -1;
        }

        log_debug("PTY created: %s -> %s", device_name, slave_name);

        return master_fd;
    }


    // Helper: open an existing device path or create a PTY if the path doesn't exist.
    // If the path exists and is a symlink (previously created PTY), recreate the PTY
    // to ensure a valid endpoint. Returns a file descriptor for the opened device.
    static int createOrOpenDevice(const char* devicePath) {
        struct stat buffer_stat;
        struct stat buffer_lstat;
        int fd = -1;

        // If the device path doesn't exist, create a PTY and symlink it here
        if (stat(devicePath, &buffer_stat) != 0) {
            fd = create_pty_device(devicePath);
        } else {
            // If the path exists, check if it's a symlink left from a previous PTY.
            // If so, remove it and create a new PTY (fresh slave) to avoid stale links.
            if (lstat(devicePath, &buffer_lstat) == 0 && S_ISLNK(buffer_stat.st_mode)) {
                if (unlink(devicePath) != 0) {
                    EmbyDebug_ASSERT_FAIL();
                }
                fd = create_pty_device(devicePath);
            } else {
                // Otherwise open the existing device node directly in non-blocking mode.
                fd = ::open(devicePath, O_RDWR | O_NOCTTY | O_NONBLOCK);
            }
        }

        if (fd < 0) {
            log_debug("Serial port open failed: %s - %s", devicePath, strerror(errno));
            EmbyDebug_ASSERT_FAIL();
        }

        return fd;
    }

    // Constructor: configure the serial device with the requested parameters.
    // This implementation targets X86/Unix and uses termios to set line discipline.
    Serial::Serial(EmbyLibs::String const &dev, Serial_Config *conf, bool doOpen) :
            m_dev(dev), m_eol(DEFAULT_EOL_CHAR), m_isOpen(false)
    {
        // Store device path in implementation struct (legacy API expects char*)
        m_impl.dev = (const_cast<char *>(m_dev.c_str()));

        // Open or create the underlying device and get a file descriptor
        m_impl.fd = createOrOpenDevice(m_impl.dev);

        // Read current termios configuration and modify it according to `conf`.
        struct termios config;
        if (tcgetattr(m_impl.fd, &config) != 0) {
            log_debug("tcgetattr failed: %s", strerror(errno));
        }

        // Clear character size bits, then set according to requested word length.
        config.c_cflag &= ~CSIZE;
        switch (conf->wordLen)
        {
            case Serial::Serial_WordLen::Serial_WordLen_7:
                config.c_cflag |= CS7; /* 7bit per byte */
                break;
            case Serial::Serial_WordLen::Serial_WordLen_8:
                config.c_cflag |= CS8;
                break;
            case Serial::Serial_WordLen::Serial_WordLen_9:
                // Linux termios doesn't support 9-bit bytes; fall back to 8-bit.
                config.c_cflag |= CS8; // 9bit is not natively supported in Linux, so we use 8bit
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        // Configure parity bits and input parity checking flags.
        switch (conf->parity)
        {
            case Serial::Serial_Parity::Serial_Parity_None:
                config.c_cflag &= ~PARENB; /* No parity */
                break;
            case Serial::Serial_Parity::Serial_Parity_Odd:
                config.c_cflag |= PARENB; /* Enable parity */
                config.c_cflag |= PARODD; /* Odd parity */
                config.c_iflag |= INPCK;  /* Enable input parity checking */
                break;
            case Serial::Serial_Parity::Serial_Parity_Even:
                config.c_cflag |= PARENB; /* Enable parity */
                config.c_iflag |= INPCK;  /* Enable input parity checking */
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        // Configure number of stop bits.
        switch (conf->stopBits)
        {
            case Serial::Serial_StopBits::Serial_StopBits_0:
                EmbyDebug_ASSERT_FAIL(); // 0 stop bits is not valid
                break;
            case Serial::Serial_StopBits::Serial_StopBits_1:
                config.c_cflag &= ~CSTOPB; /* 1 stop bit */
                break;
            case Serial::Serial_StopBits::Serial_StopBits_2:
                config.c_cflag |= CSTOPB; /* 2 stop bits */
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        // Configure mode: transmit, receive or both. CLOCAL prevents modem control
        // lines from affecting the port (we treat it like a direct serial link).
        switch (conf->mode)
        {
            case Serial::Serial_Mode::Serial_Mode_TxRx:
                config.c_cflag |= (CLOCAL | CREAD); /* Enable receiver, ignore modem control lines */
                break;
            case Serial::Serial_Mode::Serial_Mode_Tx:
                // The code historically enables CREAD even for TX-only; keep behavior.
                config.c_cflag |= (CLOCAL | CREAD); /* Enable transmitter, ignore modem control lines */
                break;
            case Serial::Serial_Mode::Serial_Mode_Rx:
                config.c_cflag &= ~(CLOCAL | CREAD); /* Disable receiver */
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        // Set baud rate using standard termios helpers.
        switch (conf->baudRate)
        {
            case Serial::Serial_BaudRate::Serial_BaudRate_115200:
                cfsetispeed(&config, B115200);
                cfsetospeed(&config, B115200);
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_19200:
                cfsetispeed(&config, B19200);
                cfsetospeed(&config, B19200);
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_4800:
                cfsetispeed(&config, B4800);
                cfsetospeed(&config, B4800);
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_57600:
                cfsetispeed(&config, B57600);
                cfsetospeed(&config, B57600);
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_9600:
                cfsetispeed(&config, B9600);
                cfsetospeed(&config, B9600);
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        // Configure hardware flow control (RTS/CTS) if requested.
        switch (conf->flowCtrl)
        {
            case Serial::Serial_FlowCtrl_None:
                config.c_cflag &= ~CRTSCTS;
                break;
            case Serial::Serial_FlowCtrl_RTS_CTS:
                config.c_cflag |= CRTSCTS;
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }


        // Apply the modified termios configuration immediately.
        if (tcsetattr(m_impl.fd, TCSANOW, &config) != 0) {
            log_debug("tcsetattr failed: %s", strerror(errno));
        }

        // Instance is created closed by default; `open()` must be called to mark it open.
        m_isOpen = false;
        m_impl.m_mutex = EmbyThreading::Mutex();

        if (doOpen)
        {
            open();
        }
    }


    // Open the Serial object for use. This registers the instance globally and
    // marks it as open. Thread-safe via a scoped mutex.
    bool Serial::open()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        if (m_isOpen) {
            return true;
        }

        g_serials.push_back(this);
        m_isOpen = true;
        flush();
        return true;
    }

    // Close the underlying file descriptor and mark the Serial closed.
    bool Serial::close()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        int retval = ::close(m_impl.fd);
        m_isOpen = false;
        return retval == 0;
    }

    // Write raw bytes to the serial device. Returns the number of bytes written.
    size_t
    Serial::write(void const *buffer, size_t length)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        if (!m_isOpen)
        {
            return 0;
        }
        EmbyDebug_ASSERT(buffer != NULL);
        return ::write(m_impl.fd,(uint8_t*)buffer,length);
    }


    // Read a single line terminated by m_eol (default EOL char). This function
    // polls the device with a small delay to avoid blocking reads and implements
    // a timeout in milliseconds. Returns an empty String on timeout or error.
    EmbyLibs::String Serial::readline(int32_t timeoutMs)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        if (!m_isOpen)
        {
            return EmbyLibs::String();
        }
        EmbyLibs::String line;
        line.clear();
        char ch;
        uint64_t start = EmbySystem::upTimeMs();
        while (true) {
            // Check how many bytes are available to read without blocking.
            int available = 0;
            if (m_impl.fd >= 0) {
                if (ioctl(m_impl.fd, FIONREAD, &available) == -1) {
                    available = 0;
                }
            }

            if (available <= 0) {
                // No data available: check timeout and sleep briefly to avoid busy loop.
                if (timeoutMs > 0 && (EmbySystem::upTimeMs() - start) > (uint64_t)timeoutMs) {
                    // Timeout reached without receiving EOL -> return empty string.
                    line.clear();
                    break;
                }
                // Avoid blocking reads; poll again after a short delay.
                EmbySystem::delayMs(5);
                continue;
            }

            // Read a single character since data is available.
            int n = ::read(m_impl.fd, &ch, 1);
            if (n > 0) {
                // NOTE: the original function didn't append characters to `line`.
                // Preserve existing behavior but document it: only EOL detection
                // is performed here; characters are not stored in `line`.
                if (ch == m_eol) {
                    break;
                }
            } else if (n == 0 || (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))) {
                // No data immediately readable; wait a bit and retry.
                EmbySystem::delayMs(5);
            } else if (n < 0) {
                // Real read error occurred; return empty result.
                line.clear();
                break;
            }
            // Check timeout after attempting to read a character.
            if (timeoutMs > 0 && (EmbySystem::upTimeMs() - start) > (uint64_t)timeoutMs) {
                line.clear();
                break;
            }
        }
        return line;
    }

    // Read up to `length` bytes into buffer. Returns number of bytes read.
    size_t Serial::read(void *buffer, size_t length)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        if (!m_isOpen)
        {
            return 0;
        }
        int size = ::read(m_impl.fd, (uint8_t*)buffer,length);
        return size >= 0 ? size : 0;
    }

    // Enable reception on the serial port (sets CREAD flag in termios).
    void Serial::enableRx()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        if (!m_isOpen)
        {
            return;
        }
        struct termios config;
        if (tcgetattr(m_impl.fd, &config) == 0) {
            config.c_cflag |= CREAD;
            tcsetattr(m_impl.fd, TCSANOW, &config);
        }
    }


    // Flush both input and output hardware buffers.
    void Serial::flush()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        if (!m_isOpen)
        {
            return;
        }
        tcflush(m_impl.fd, TCIOFLUSH); // Flush input and output hardware buffers
    }


    // Disable reception on the serial port by clearing CREAD.
    void Serial::disableRx()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        if (!m_isOpen)
        {
            return;
        }
        struct termios config;
        if (tcgetattr(m_impl.fd, &config) == 0) {
            config.c_cflag &= ~CREAD;
            tcsetattr(m_impl.fd, TCSANOW, &config);
        }
    }

    // Placeholder: get the status of a hardware pin (RTS/CTS, etc.). Not
    // implemented for the Unix X86 platform and will assert if used.
    Serial::Serial_Pin_Status Serial::getPinStatus(Serial_Hw_Pin pin)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        if (!m_isOpen)
        {
            return Serial::Serial_Pin_Status::Serial_Pin_Unkwown;
        }
        EmbyDebug_ASSERT_FAIL();
        return Serial::Serial_Pin_Status::Serial_Pin_Unkwown;
    }


    // Placeholder: set hardware pin status. Not implemented for X86/Unix.
    bool Serial::setPinStatus(Serial_Hw_Pin pin, bool status)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        if (!m_isOpen)
        {
            return false;
        }
        EmbyDebug_ASSERT_FAIL();
        return false;
    }

    // sendBreak: platform-specific sending of a break condition. Not
    // implemented on this platform.
    void Serial::sendBreak()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        if (!m_isOpen)
        {
            return;
        }
    }

    // Return number of bytes immediately available to read without blocking.
    int Serial::getAvailableBytes()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        if (!m_isOpen)
        {
            return 0;
        }
        int bytes = 0;
        if (m_impl.fd >= 0) {
            if (ioctl(m_impl.fd, FIONREAD, &bytes) == -1) {
                bytes = 0;
            }
        }
        return bytes;
    }
}

