
#include <EmbyMachine/Serial.hh>
#include <EmbyMachine/SerialImpl.hh>

#include <EmbyLibs/CircularBuffer.hh>

#include <EmbyDebug/assert.hh>
#include <EmbyThreading/Thread.hh>
#include <EmbyThreading/ScopedMutex.hh>
#include <EmbyThreading/Worker.hh>
#include <EmbyLibs/StringUtils.hh>

using namespace EmbyExec;
using namespace EmbyLibs;



static std::vector<EmbyMachine::Serial *> g_serials;

extern "C"
{
#include "Uart_Config.h"
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

//    int create_pty_pair(int *master_fd, int *slave_fd, char *slave_name, size_t name_len) {
//        if (master_fd == NULL || slave_fd == NULL || slave_name == NULL) {
//            EmbyDebug_ASSERT_FAIL();
//        }
//
//        memset(slave_name, 0, name_len);
//
//        if (openpty(master_fd, slave_fd, slave_name, NULL, NULL) == -1) {
//            EmbyDebug_ASSERT_FAIL();
//        }
//
//        return 0;
//    }

    int create_pty_device(const char* device_name) {
        int master_fd, slave_fd;
        char *slave_name;
        // Apri il master PTY
        master_fd = posix_openpt(O_RDWR | O_NOCTTY);
        if (master_fd < 0) return -1;
        // Sblocca e imposta il slave
        grantpt(master_fd);
        unlockpt(master_fd);
        // Ottieni il nome del slave
        slave_name = ptsname(master_fd);
        // Crea un link simbolico con il nome desiderato
        symlink(slave_name, device_name);
        return master_fd; // Il server userà questo fd
    }

    void ensureDeviceExists(const char* devicePath)
    {
        struct stat buffer;
        if (stat(devicePath, &buffer) != 0) {
            // Device does not exist, create it
            int majorNumber = MAJOR_NUMBER; // Replace with the correct major number
            int minorNumber = MINOR_NUMBER;   // Replace with the correct minor number
            dev_t dev = makedev(majorNumber, minorNumber);
            if (mknod(devicePath, S_IFCHR | 0666, dev) != 0) {
                EmbyDebug_ASSERT_FAIL();
            }
        }
    }


    Serial::Serial(EmbyLibs::String const &dev, Serial_Config *conf, bool doOpen) :
            m_dev(dev), m_eol(DEFAULT_EOL_CHAR), m_isOpen(false)
    {
        m_impl.dev = (const_cast<char *>(m_dev.c_str()));

        struct termios config;
        if (tcgetattr(m_impl.fd, &config) != 0) {
            perror("tcgetattr");
        }
        config.c_cflag &= ~CSIZE;
        switch (conf->wordLen)
        {
            case Serial::Serial_WordLen::Serial_WordLen_7:
                config.c_cflag |= CS7; /* 7bit x byte */
                break;
            case Serial::Serial_WordLen::Serial_WordLen_8:
                config.c_cflag |= CS8;
                break;
            case Serial::Serial_WordLen::Serial_WordLen_9:
                config.c_cflag |= CS8; // 9bit is not natively supported in Linux, so we use 8bit
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        switch (conf->parity)
        {
            case Serial::Serial_Parity::Serial_Parity_None:
                config.c_cflag &= ~PARENB; /* No parity */
                break;
            case Serial::Serial_Parity::Serial_Parity_Odd:
                config.c_cflag |= PARENB; /* Enable parity */
                config.c_cflag |= PARODD; /* Odd parity */
                config.c_iflag |= INPCK;  /* Input parity check */
                break;
            case Serial::Serial_Parity::Serial_Parity_Even:
                config.c_cflag |= PARENB; /* Enable parity */
                config.c_iflag |= INPCK;  /* Input parity check */
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

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
        switch (conf->mode)
        {
            case Serial::Serial_Mode::Serial_Mode_TxRx:
                config.c_cflag |= (CLOCAL | CREAD); /* Enable receiver, ignore modem control lines */
                break;
            case Serial::Serial_Mode::Serial_Mode_Tx:
                config.c_cflag |= (CLOCAL | CREAD); /* Enable transmitter, ignore modem control lines */
                break;
            case Serial::Serial_Mode::Serial_Mode_Rx:
                config.c_cflag &= ~(CLOCAL | CREAD); /* Disable receiver */
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

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

        // Uart_ctorNopen(&m_impl.m_uart, Uart_Num_1, &m_impl.m_cfg, 0, 0);
        m_isOpen = false;
        m_impl.m_mutex = EmbyThreading::Mutex();
        m_impl.m_discarded = 0;
        m_impl.m_buff = EmbyLibs::CircularBuffer<uint8_t, APP_CIRC_BUFF_SIZE>();  //app buffer

        if (doOpen)
        {
            open();
        }
    }


    bool Serial::open()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        if (m_isOpen)
        {
            return true;
        }
        g_serials.push_back((this));

        struct stat buffer_stat;
        struct stat buffer_lstat;

        if (stat(m_impl.dev, &buffer_stat) != 0)
        {

            m_impl.fd = create_pty_device(m_impl.dev);
        }
        else
        {
            if (lstat(m_impl.dev, &buffer_lstat) == 0 && S_ISLNK(buffer_stat.st_mode))
            {
                if (unlink(m_impl.dev) != 0)
                {
                    EmbyDebug_ASSERT_FAIL();
                }
                m_impl.fd = create_pty_device(m_impl.dev);
            }
            else{
                m_impl.fd = ::open(m_impl.dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
            }
        }

        if (m_impl.fd < 0)
        {
            fprintf(stderr, "Error opening serial port %s: %s\n", m_impl.dev, strerror(errno));
            EmbyDebug_ASSERT_FAIL();
        }
        if (m_impl.fd >= 0){
            m_isOpen = true;
        }
        return m_isOpen;
    }

    bool Serial::close()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        int retval = ::close(m_impl.fd);
        m_isOpen = false;
        return retval == 0;
    }

    void
    Serial::sendBreak()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
    }

    size_t
    Serial::write(void const *buffer, size_t length)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        EmbyDebug_ASSERT(buffer != NULL);
        return ::write(m_impl.fd,(uint8_t*)buffer,length);
    }


	static int checkLineInTheBuffer(EmbyMachine::Serial* s )
	{
		size_t len = 0;

		volatile uint32_t i = 0;

		if( ! s->getImpl()->m_buff.isEmpty() )
		{
	//		for(auto it =  s->getImpl()->m_buff.begin() ;
	//			it != s->getImpl()->m_buff.end(); it++)
			for(auto c :  s->getImpl()->m_buff )
			{
				i++;
				EmbyDebug_ASSERT(i <= s->getImpl()->m_buff.getCapacity());
				//volatile auto c = *it;
				if( c == s->getEndofLineChar() )
				{
					len = i;
					break;
				}
			}
		}
		return len;
	}

	static bool readLineInsideBuffer(EmbyLibs::String& line , EmbyMachine::Serial* s)
	{
		EmbyDebug_ASSERT_CHECK_NULL_PTR( s );
		bool retval = false;
		//readline
	//	line.clear();
		int toPop = 0;
		bool dothrow = false;

		EmbySystem_BEGIN_CRITICAL_SECTION;
		if( ! s->getImpl()->m_buff.isEmpty())
		{
			EmbyDebug_ASSERT_CHECK_NULL_PTR(&s->getImpl()->m_buff);

			for(auto c :  s->getImpl()->m_buff )
			{
				toPop++;
				if( c != s->getEndofLineChar() )
				{
					try
					{
						//could throw excep
						line.push_back(c);
					}
					catch (...)
					{
						dothrow = true;
					}
				}
				else
				{
					retval = true;
					break;
				}
			}

			for(int z= 0 ; z < toPop ; z++)
			{
				if(!s->getImpl()->m_buff.isEmpty())
				{
					s->getImpl()->m_buff.pop();
				}
			}
		}
		EmbySystem_END_CRITICAL_SECTION;

		if(dothrow)
		{
			throw std::bad_alloc();
		}

		return retval;
	}

    EmbyLibs::String Serial::readline(int32_t timeoutMs)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);

        volatile bool exit = false;
        volatile size_t len = 0;
        String line;
        line.clear();

        uint64_t start = EmbySystem::upTimeMs();

        do
        {
            len = checkLineInTheBuffer(this);

            if (len >= 0)
            {

                auto err = line.reserve(len);
                if (err < 0)
                {
                    line.clear();
                    return line;
                }
                exit = readLineInsideBuffer(line, this);
            }
            else
            {
                EmbySystem::delayMs(5);
            }

            if (timeoutMs > 0)
            {
                if ((EmbySystem::upTimeMs() - start) > timeoutMs)
                {
                    line.clear();
                    return line;
                }
            }

        } while (!exit);

        return line;
    }

    size_t Serial::read(void *buffer, size_t length)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        int size = ::read(m_impl.fd, (uint8_t*)buffer,length);
        return size >= 0 ? size : 0;
    }

    void Serial::enableRx()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
    }


    void Serial::flush()
    {
        EmbySystem_BEGIN_CRITICAL_SECTION;
        {
            m_impl.m_buff.clear();
        }
        EmbySystem_END_CRITICAL_SECTION;
    }


    void Serial::disableRx()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
    }

    Serial::Serial_Pin_Status Serial::getPinStatus(Serial_Hw_Pin pin)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        EmbyDebug_ASSERT_FAIL();
        return Serial::Serial_Pin_Status::Serial_Pin_Unkwown;
    }


    bool Serial::setPinStatus(Serial_Hw_Pin pin, bool status)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        EmbyDebug_ASSERT_FAIL();
        return false;
    }

    int Serial::getAvailableBytes()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        int bytes = 0;

        EmbySystem_BEGIN_CRITICAL_SECTION;
        {
            bytes = m_impl.m_buff.getSize();
        }
        EmbySystem_END_CRITICAL_SECTION;

        return bytes;
    }
}

