
#if !defined(SERIAL_IMPLH_H)
#define SERIAL_IMPLH_H


//#include <select.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <EmbyMachine/UartLinux/Uart.h>
#include <EmbyThreading/Mutex.hh>
#include <EmbyLibs/CircularBuffer.hh>


struct SerialImpl
{
    EmbyThreading::Mutex m_mutex;
    int fd;
    char *dev;
};


#endif /* UART_ARCH_H */