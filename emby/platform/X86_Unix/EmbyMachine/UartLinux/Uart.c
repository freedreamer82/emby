#include "Uart.h"
#include "Uart_Config.h"
#include "Uart_Impl.h"
#include "stdbool.h"
#include "string.h"
#include <sys/time.h>
#include "pthread.h"
#include "unistd.h"
#include "sys/ioctl.h"
#include <fcntl.h>

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

#ifndef  Debug_ASSERT
    #define Debug_ASSERT (void)
#endif

#ifndef FNDELAY
#define FNDELAY O_NONBLOCK
#endif

#define CCTS_OFLOW	0x00010000	/* CTS flow control of output */
//#define CRTSCTS		CCTS_OFLOW	/* ??? */
#define CRTS_IFLOW	0x00020000	/* RTS flow control of input */

/* Public functions ----------------------------------------------------------*/

static void*
Uart_taskRun(void* ctx)
{
    Uart* self = (Uart*)ctx;
    int fdReady = 0;
    struct timeval timeOut;
    memset(&timeOut, 0, sizeof(timeOut));

    while(self->impl.taskIsrunning)
    {
        if (self->impl.devIsOpen)
        {
            FD_SET(self->impl.fd, &self->impl.fdReadMask); 
            FD_SET(self->impl.fd, &self->impl.fdErrMask);    
           /*FD_SET(self->impl.fd, &self->impl.fdWriteMask); */

            /*Timeout MUST be Reset every TIME!! */
            timeOut.tv_sec  = 5;
            timeOut.tv_usec = 0;

            fdReady = select(self->impl.fd + 1, 
                            &self->impl.fdReadMask,
                            /*&self->impl.fdWriteMask,*/
                            NULL,
                            &self->impl.fdErrMask,
                            &timeOut);
            if (fdReady > 0)
            {
                if (FD_ISSET(self->impl.fd , &self->impl.fdReadMask))
                {
                    if (self->callback != NULL)
                    {
                        self->callback(self->callbackCtx , self);
                    }
                }
                if (FD_ISSET(self->impl.fd , &self->impl.fdErrMask))
                {
                    /*@todo: do something..*/
                }
            }
            else
            {

            }
        }
        else
        {
            sleep(1);
        }
    }
    return NULL;
}

size_t
Uart_read(Uart* self, void* buffer, size_t length)
{	
    int size = read(self->impl.fd, (uint8_t*)buffer,length);
    return size >= 0 ? size : 0;
}

void
routeRx(void* context, Uart* uart)
{
    (void) uart;
    size_t readBytes = 0, writtenBytes = 0;
    char buffer[16];

    do
    {
        readBytes = Uart_read(uart, buffer, sizeof(buffer));
        writtenBytes = 0;

        while (writtenBytes < readBytes)
        {
            writtenBytes += Uart_write((Uart*) context,
                                       &buffer[writtenBytes],
                                       readBytes - writtenBytes);
        }
    } while (readBytes > 0);
}

Uart_Err
Uart_config(Uart* self, const Uart_Config* config)
{
    Debug_ASSERT(self && config != NULL);

    Uart_Err err = Uart_Err_Unknown;
   
    struct termios configSerial;
    memset(&configSerial,0,sizeof(configSerial));

    cfmakeraw(&configSerial);
    configSerial.c_cflag		|= CLOCAL;

    switch (config->flowCtrl)
    {
       /*Hw flow ctrl*/ 
        case Uart_FlowCtrl_None:
             /*do nothing*/
        break;
        case Uart_FlowCtrl_RTS_CTS:
             configSerial.c_cflag |= CCTS_OFLOW; /*CTS*/
             configSerial.c_cflag |= CRTS_IFLOW; /*RTS*/
        break;
        case Uart_FlowCtrl_RTS:
             configSerial.c_cflag |= CRTS_IFLOW; /*RTS*/
        break;
        case Uart_FlowCtrl_CTS:
             configSerial.c_cflag |= CCTS_OFLOW; /*CTS*/
        break; 
        /*Sw flow Ctrl*/
        case Uart_FlowCtrl_SW_RTS_CTS:
             err = Uart_Err_Config_FlowCtrl;
             goto exit;
        break;
        case Uart_FlowCtrl_SW_RTS:
             err = Uart_Err_Config_FlowCtrl;
             goto exit;
        break;
        case Uart_FlowCtrl_SW_CTS:
             err = Uart_Err_Config_FlowCtrl;
             goto exit;
        break; 

        default:
            err = Uart_Err_Config_FlowCtrl;
        goto exit;
    }
       
    switch (config->wordLen)
    {
        case Uart_WordLen_7:
            configSerial.c_cflag |= CS7; /*7bit x byte*/
            break;
        case Uart_WordLen_8:
            configSerial.c_cflag |= CS8; /*8bit x byte*/
            break;
            
        default:
            err = Uart_Err_Config_WorldLen;
        goto exit;
    }

    switch (config->parity)
    {
        case Uart_Parity_None:
             /*do nothing*/
        break;
        case Uart_Parity_Odd:
            configSerial.c_cflag |= PARENB; /*enable parity*/
            configSerial.c_cflag |= PARODD; /*enable parity*/
            configSerial.c_iflag |= INPCK; /*input parity check*/
        break;
        case Uart_Parity_Even:
            configSerial.c_cflag |= PARENB; /*enable parity , implicit is even*/
            configSerial.c_iflag |= INPCK; /*input parity check*/
        break;
    
        default:
            err = Uart_Err_Config_Parity;
        goto exit;
    }

    switch (config->stopBits)
    {
        case Uart_StopBits_0:
            err = Uart_Err_Config_StopBits;
            goto exit;
        break;
        case Uart_StopBits_1:
        	  /*do nothing*/
        break;
        case Uart_StopBits_2:
           configSerial.c_cflag |= CSTOPB; 
        break;
        default:
            err = Uart_Err_Config_StopBits;
        goto exit;
    }
    
    switch (config->mode)
    {
        case Uart_Mode_TxRx:
           /*  init.USART_Mode = USART_Mode_Rx | USART_ModeTx;*/
            configSerial.c_cflag |= CREAD;
        break;
        case Uart_Mode_Rx:
           /* init.USART_Mode = USART_Mode_Rx;*/
        break;
        case Uart_Mode_Tx:
            configSerial.c_cflag |= ~ CREAD; /*rx not enable*/
        break;
        default:
            err = Uart_Err_Config_Mode;
        goto exit;
    }

    speed_t speed;
    switch (config->baudRate) 
    {
        case 50:     speed = B50;   break;
        case 110:    speed =  B110; break;
        case 134:    speed =  B134; break;
        case 150:    speed =  B150; break;
        case 200:    speed =  B200; break;
        case 300:    speed =  B300; break;
        case 600:    speed =  B600; break;
        case 1200:   speed =  B1200; break;
        case 1800:   speed =  B1800; break;
        case 2400:   speed =  B2400; break;
        case 4800:   speed =  B4800; break;
        case 9600:   speed =  B9600; break;
        case 19200:  speed =  B19200; break;
        case 38400:  speed =  B38400; break;
        case 115200: speed =  B115200; break;
        default:
            err = Uart_Err_Config_BaudRate;
        goto exit;
        break;
    }
    
    if ( (cfsetispeed(&configSerial,speed) == 0)&&
         (cfsetospeed(&configSerial,speed)== 0) )
         
    {
        /* everything is ok */
        err = Uart_Err_None;
    }
    
exit:
    return err;
}


bool
Uart_ctor(Uart*        self,
          Uart_Num     uartNum,
          Uart_Config* config,               
          uint16_t     writeBufferSize,
          uint16_t     readBufferSize)
{
    Debug_ASSERT( uartNum < Uart_Num_Unkwown && self != NULL  &&  config != NULL);
    (void)writeBufferSize;
    (void)readBufferSize;
    bool ok = false;
    self->impl.dev = NULL;

    switch (uartNum)
	{
	   case Uart_Num_1:
		   self->impl.dev = Uart_Config_Uart_Num_1_dev;
	   break;
	   case Uart_Num_2:
		   self->impl.dev = Uart_Config_Uart_Num_2_dev;
	   break;
	   case Uart_Num_3:
		   self->impl.dev = Uart_Config_Uart_Num_3_dev;
	   break;
	   case Uart_Num_4:
		   self->impl.dev = Uart_Config_Uart_Num_4_dev;
	   break;
	   case Uart_Num_5:
		   self->impl.dev = Uart_Config_Uart_Num_5_dev;
	   break;
	   case Uart_Num_6:
		   self->impl.dev = Uart_Config_Uart_Num_6_dev;
	   break;
	   case Uart_Num_7:
		   self->impl.dev = Uart_Config_Uart_Num_7_dev;
	   break;
	   case Uart_Num_8:
		   self->impl.dev = Uart_Config_Uart_Num_8_dev;
	   break;

	   default:
		   ok= false;
	   goto exit;
	}

    if (Uart_config(self, config) == Uart_Err_None)
    {
		/*start config parameters..*/
			self->uartNum            = uartNum;
			self->callback           = NULL;
			self->callbackCtx        = NULL;
            self->impl.devIsOpen     = false;
            self->impl.taskIsrunning = true;
            ok = true;
	}

exit:
	return ok;
}

bool
Uart_ctorNopen(Uart*        self,
                Uart_Num     uartNum,
                Uart_Config* config,               
                uint16_t     writeBufferSize,
                uint16_t     readBufferSize)
{
    return Uart_ctor(self,uartNum,config,writeBufferSize,readBufferSize) && 
           Uart_open(self);
}

void
Uart_dtor(Uart* self)
{
    Debug_ASSERT(self != NULL);
    
    Uart_close(self);
    self->impl.taskIsrunning = false;
    pthread_exit(&self->impl.pThread);
    memset(self,0,sizeof(Uart));
    
    self = NULL;
}


bool
Uart_open( Uart* self )
{
    Debug_ASSERT(self!= NULL);    

    bool   retval = false;
    struct termios config;
    self->impl.fd = open(self->impl.dev, O_RDWR | O_NONBLOCK);
    if (self->impl.fd >= 0)
    {
        /*Save Old configuration*/
        if (  tcgetattr(self->impl.fd, &config)             ||
              tcsetattr(self->impl.fd,TCSANOW,&config)      ||
              tcsetattr(self->impl.fd,TCSAFLUSH,&config)    ||
              fcntl(self->impl.fd ,F_SETFL, FNDELAY)        || /*read non blocking*/
              fcntl(self->impl.fd, F_SETFL, O_NONBLOCK) )       /*NON blocking file...*/
        {
            close(self->impl.fd);
            retval = false;
        }
        else
        {
        	retval = pthread_create(&self->impl.pThread ,  NULL, Uart_taskRun , self)== 0;
            /*retval = true;*/
        	self->impl.devIsOpen = true;
        }
    }    
    return retval;
}

bool
Uart_close( Uart* self )
{
    Debug_ASSERT(self!= NULL);
	int retval = close(self->impl.fd);
    return retval == 0;
}



int
Uart_getUnit(Uart const* self)
{
    Debug_ASSERT(self!= NULL);
    return self->uartNum;
}

size_t
Uart_inWaiting(Uart* self)
{
    Debug_ASSERT(self!=NULL);
    int size;
    int retval = ioctl(self->impl.fd, FIONREAD , &size);
    return  retval >= 0 ? size : 0;
}

size_t
Uart_outWaiting(Uart* self)
{
    Debug_ASSERT(self!=NULL);
    int size;
    int retval = ioctl(self->impl.fd, TIOCOUTQ, &size);
    return  retval >= 0 ? size : 0;
}


void
Uart_enableRx(Uart* self)
{
    Debug_ASSERT(self!=NULL);
    

}


void
Uart_disableRx(Uart* self)
{
    Debug_ASSERT(self!=NULL);
}


bool
Uart_writeSync(Uart* self, void const* buffer,
               size_t length)
{
    Debug_ASSERT(self != NULL && buffer != NULL);
	
	bool retval = false;
	int flags = fcntl(self->impl.fd, F_GETFL);
	fcntl(self->impl.fd, F_SETFL, flags & (~ O_NONBLOCK)); /*make it sync*/
	
	if ( write(self->impl.fd,(uint8_t*)buffer,length) > 0 )
	{
		retval = true;
	}
	
	fcntl(self->impl.fd,F_SETFL, O_NONBLOCK);       /* make it non-blocking*/
	
	return retval;
}

size_t
Uart_write(Uart* self, void const* buffer, size_t length)
{
    Debug_ASSERT(self != NULL && buffer != NULL);
    return write(self->impl.fd,(uint8_t*)buffer,length);
}

void
Uart_setCallback(Uart* self, Uart_Callback* callback, void* context)
{
    Debug_ASSERT(self!= NULL && callback != NULL);
    self->callback      = callback;
    self->callbackCtx   = context;
}

void
Uart_setBridge(Uart* first, Uart* second)
{
    Debug_ASSERT(first!= NULL && second != NULL);
    Uart_setCallback(first, routeRx, second);
    Uart_setCallback(second, routeRx, first);
}


Uart_Pin_Status
Uart_getPinStatus(Uart* self , Uart_Hw_Pin pin)
{
    Debug_ASSERT(self!= NULL);

    Uart_Pin_Status retval= Uart_Pin_Unkwown;
    int pinStatus;
    if ( ioctl(self->impl.fd, TIOCMGET, &pinStatus) == 0)
    {
        switch(pin)
        {
            case Uart_Hw_Pin_CTS:
                retval = ((pinStatus & TIOCM_CTS) == 0) ? Uart_Pin_Low: Uart_Pin_High;
            break;
            /*
            case Uart_Hw_Pin_RTS:
                retval = BSP_USART_getRTS(self->impl.usart);
            break;
            case Uart_Hw_Pin_DTR:
                retval = BSP_USART_getDTR(self->impl.usart);
            break;
            case Uart_Hw_Pin_DCD:
                retval = BSP_USART_getDCD(self->impl.usart);
            case Uart_Hw_Pin_RING:
                retval = BSP_USART_getRING(self->impl.usart);
            break;
            case Uart_Hw_Pin_TX:
                retval = BSP_USART_getTX(self->impl.usart);
            break;
            case Uart_Hw_Pin_RX:
                retval = BSP_USART_getRX(self->impl.usart);
            break;*/

            default:
                retval = Uart_Pin_Unkwown;
            break;
        }
    }
    return retval;
    
}
    

bool
Uart_setPinStatus(Uart* self , Uart_Hw_Pin pin , bool status)
{
    (void)status;
    Debug_ASSERT(self!= NULL);

    Uart_Pin_Status retval= Uart_Pin_Unkwown;
    int pinStatus;
    if ( ioctl(self->impl.fd, TIOCMGET, &pinStatus) == 0)
    {
        switch(pin)
        {
        /*
            case Uart_Hw_Pin_CTS:
                retval = (pinStatus |= TIOCM_RTSCTS) ? Uart_Pin_Low: Uart_Pin_High;
            break;
            case Uart_Hw_Pin_RTS:
                retval = BSP_USART_getRTS(self->impl.usart);
            break;
            case Uart_Hw_Pin_DTR:
                retval = BSP_USART_getDTR(self->impl.usart);
            break;
            case Uart_Hw_Pin_DCD:
                retval = BSP_USART_getDCD(self->impl.usart);
            case Uart_Hw_Pin_RING:
                retval = BSP_USART_getRING(self->impl.usart);
            break;
            case Uart_Hw_Pin_TX:
                retval = BSP_USART_getTX(self->impl.usart);
            break;
            case Uart_Hw_Pin_RX:
                retval = BSP_USART_getRX(self->impl.usart);
            break;
            */

            default:
                retval = Uart_Pin_Unkwown;
            break;
        }
    }
    return retval;
}




/*------------------ (C) COPYRIGHT 2009 MR&D Institute -------- END OF FILE --*/
