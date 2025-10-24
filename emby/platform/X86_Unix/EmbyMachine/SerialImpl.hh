
#if !defined(SERIAL_IMPLH_H)
#define SERIAL_IMPLH_H


//#include <select.h>
#include <fcntl.h>
//#include <termios.h>
#include <pthread.h>
#include <EmbyMachine/UartLinux/Uart.h>
#include <EmbyThreading/Mutex.hh>


typedef struct SerialImpl SerialImpl;
struct SerialImpl
{
    Uart_Config m_cfg;
    Uart m_uart;
    EmbyThreading::Mutex m_mutex;
//      EmbyLibs::CircularBuffer<uint8_t,APP_CIRC_BUFF_SIZE> m_buff;  //app buffer
    uint32_t m_discarded;
//        unsigned char		   m_char;
//    pthread_t pThread;
//    int fd;
//    fd_set fdReadMask;
//    fd_set fdErrMask;
//    fd_set fdWriteMask;
//    char *dev;
//    bool devIsOpen;
//    bool taskIsrunning;
};


#endif /* UART_ARCH_H */


