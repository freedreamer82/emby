#if !defined(UART_IMPLH_H)
#define UART_IMPLH_H


/* Includes ------------------------------------------------------------------*/

#include <sys/select.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>

/* Exported macro ------------------------------------------------------------*/

#ifdef __cplusplus

extern "C"
{

#endif

/* Exported types ------------------------------------------------------------*/

typedef struct Uart_Impl Uart_Impl;
struct Uart_Impl
{    
    pthread_t   pThread;
    int         fd;
    fd_set      fdReadMask;
    fd_set      fdErrMask;
    fd_set      fdWriteMask;
    char*       dev;
    bool        devIsOpen;
    bool        taskIsrunning;
};

/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#ifdef __cplusplus

}

#endif


#endif /* UART_ARCH_H */


