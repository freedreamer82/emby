#include <EmbyMachine/Serial.hh>
#include <EmbyLibs/String.hh>
#include <cassert>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

using namespace EmbyMachine;
using namespace EmbyLibs;

int main()
{
    std::cout << "Serial device test: init.." << std::endl;

    const char *device = "/tmp/emby_test_tty0";

    // Configure a sensible default serial configuration
    Serial::Serial_Config conf;
    conf.wordLen = Serial::Serial_WordLen::Serial_WordLen_8;
    conf.parity = Serial::Serial_Parity::Serial_Parity_None;
    conf.stopBits = Serial::Serial_StopBits::Serial_StopBits_1;
    conf.flowCtrl = Serial::Serial_FlowCtrl_None;
    conf.baudRate = Serial::Serial_BaudRate::Serial_BaudRate_9600;
    conf.mode = Serial::Serial_Mode::Serial_Mode_TxRx;

    // Create Serial which will create a PTY master and a symlink at `device`.
    Serial serial(String(device), &conf, true);
    assert(serial.isOpen());

    // Open the slave side (the symlink points to the real ptsN device)
    // Give a small delay to ensure the symlink has been created.
    usleep(10000);
    int slave_fd = ::open(device, O_RDWR | O_NOCTTY);
    if (slave_fd < 0) {
        std::cerr << "Failed to open slave device: " << device << " errno=" << strerror(errno) << std::endl;
        return 1;
    }

    // Test 1: write on slave, read via Serial (master)
    const char *msg1 = "hello_serial\n";
    ssize_t w = ::write(slave_fd, msg1, strlen(msg1));
    assert(w == (ssize_t)strlen(msg1));

    // Wait for bytes to be available
    int tries = 0;
    while (serial.getAvailableBytes() == 0 && tries++ < 50) {
        usleep(1000);
    }
    assert(serial.getAvailableBytes() > 0);

    char buffer[64];
    memset(buffer, 0, sizeof(buffer));
    size_t r = serial.read(buffer, sizeof(buffer)-1);
    assert(r > 0);
    std::string got(buffer, r);
    // Because readline in this implementation doesn't append, we expect raw read
    // to return the data we wrote. Check prefix
    assert(got.find("hello_serial") != std::string::npos);

    // Test 2: write from Serial (master) and read on slave
    const char *msg2 = "reply_from_master\n";
    size_t written = serial.write(msg2, strlen(msg2));
    assert(written == strlen(msg2));

    // Read from slave to verify
    memset(buffer, 0, sizeof(buffer));
    ssize_t r2 = ::read(slave_fd, buffer, sizeof(buffer)-1);
    assert(r2 > 0);
    std::string got2(buffer, r2);
    assert(got2.find("reply_from_master") != std::string::npos);

    // Cleanup
    ::close(slave_fd);
    serial.close();
    unlink(device);

    std::cout << "\nTEST SerialDevice OK!!" << std::endl;
    return 0;
}

