/******************** (C) COPYRIGHT 2009 MR&D Institute ***********************/
/**
 * @file Uart.h
 *
 * @brief <brief description>
 *
 * <detailed description, on one or more rows>
 *
 * @author Garzola Marco & MrD Team
 * @version <00.01>
 * @date 8/10/2012
 * @note None
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/

#if !defined(UART_H)
#define UART_H


#ifdef __cplusplus

extern "C"
{

#endif



/* Includes ------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdint.h>
#include "stdbool.h"
#include "Uart_Impl.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    Uart_Num_1,
    Uart_Num_2,
    Uart_Num_3,
    Uart_Num_4,
    Uart_Num_5,
    Uart_Num_6,
    Uart_Num_7,
    Uart_Num_8,
    Uart_Num_Unkwown
}
Uart_Num;

typedef enum
{
    Uart_WordLen_7 = 7,
    Uart_WordLen_8,
    Uart_WordLen_9
}
Uart_WordLen;

typedef enum
{
    Uart_StopBits_0,
    Uart_StopBits_1,
    Uart_StopBits_2
}
Uart_StopBits;

typedef enum
{
    Uart_Parity_None,
    Uart_Parity_Odd,
    Uart_Parity_Even
}
Uart_Parity;

typedef enum
{
    Uart_FlowCtrl_None,
    Uart_FlowCtrl_RTS_CTS,
    Uart_FlowCtrl_RTS,
    Uart_FlowCtrl_CTS,
    Uart_FlowCtrl_SW_RTS_CTS,
    Uart_FlowCtrl_SW_RTS,
    Uart_FlowCtrl_SW_CTS,
}
Uart_FlowCtrl;

typedef enum
{
    Uart_Mode_TxRx,
    Uart_Mode_Tx,
    Uart_Mode_Rx
}
Uart_Mode;

typedef enum
{
    Uart_Err_None,
    Uart_Err_Config_FlowCtrl,
    Uart_Err_Config_WorldLen,
    Uart_Err_Config_Parity,
    Uart_Err_Config_StopBits,
    Uart_Err_Config_BaudRate,
    Uart_Err_Config_Mode,
    Uart_Err_Unknown
}
Uart_Err;


typedef struct
{
    uint8_t     wordLen;
    uint8_t     parity;
    uint8_t     stopBits;
    uint8_t     flowCtrl;
    uint32_t    baudRate;
    uint8_t     mode;
}
Uart_Config;

typedef enum
{
    Uart_Pin_Low ,
    Uart_Pin_High,
    Uart_Pin_Unkwown,
    Uart_Pin_NotConnected
}
Uart_Pin_Status;


typedef enum
{
    Uart_Hw_Pin_CTS,
    Uart_Hw_Pin_RTS,
    Uart_Hw_Pin_DTR,
    Uart_Hw_Pin_DCD,
    Uart_Hw_Pin_RING,
    Uart_Hw_Pin_TX,
    Uart_Hw_Pin_RX,
    Uart_Hw_Pin_DSR
}
Uart_Hw_Pin;


typedef struct Uart Uart;
/**
 * Uart_Callback.
 *
 * This is the signature of the function invoked when any amount of data is
 * available to be read from the serial line.
 *
 * @param context a user supplied context pointer.
 * @param uart a pointer to the uart where data is available.
 *
 */

typedef void (Uart_Callback)(void* context, Uart* uart);

struct Uart
{
    /** privates */    
    Uart_Callback*      callback;
    void*               callbackCtx;
    uint8_t             uartNum;
    Uart_Impl           impl;
};


/* Exported funct ------------------------------------------------------------*/

/**
 * Uart communication constructor.
 * This constructor initializes the uart port. You must take care of
 * not using the same port twice.
 *
 * @param self a pointer to the Uart object to construct.
 * @param usartUnit the number of the uart unit you want to access.
 * @param Uart_Config configuration parameters.
 * @param writeBufferSize write Buffer Size.
 * @param readBufferSize  read  Buffer Size.
 * @return true if the constructor has been successful, false otherwise.
 */
bool 
Uart_ctor( Uart*        self,
           Uart_Num     uartNum,
           Uart_Config* config,               
           uint16_t     writeBufferSize,
           uint16_t     readBufferSize);

/**
 * Same as Uart_ctor but also open(enable) the peripheral
 *
 */
bool
Uart_ctorNopen( Uart*        self,
                Uart_Num     uartNum,
                Uart_Config* config,               
                uint16_t     writeBufferSize,
                uint16_t     readBufferSize);

/**
 *  This function enable the serial port.
 *  @param self a pointer to the Uart object.
 */
bool
Uart_open( Uart* self );

/**
 * This function disables the serial port.
 * @param self a pointer to the Uart object.
 */
bool
Uart_close( Uart* self );



/**
 * Uart destructor.
 * This function closes the serial port.
 *
 * @param self a pointer to the Uart object.
 */
void
Uart_dtor( Uart* self );



/**
 * Writes a buffer to the serial line. This call is asynchronous: characters
 * are stored into an internal buffer and then discarded.
 *
 * @param self a pointer to the Uart object.
 * @param buffer a pointer to the sequence of characters to write.
 * @param length number of characters to write.
 */
size_t
Uart_write( Uart* self, void const* buffer, size_t length );


/**
 * Reads From the Uart Buffer up to bufferSize bytes.
 * Returns the actually read bytes. This function never blocks the caller:
 * if no data are available 0 is returned
 *
 * @param self a pointer to the Uart object.
 * @param buffer a pointer to the sequence of characters to read.
 * @param bufferSize max size of the bytes to read.
 */
size_t
Uart_read( Uart* self , void* buffer , size_t bufferSize);



/**
 * write Sync. It waits the shift register is empty after length bytes have been 
 * written.
 */
bool
Uart_writeSync(Uart* self,void const* buffer, size_t length);


/**
 * Sets the callback function.
 *
 * This method can be used to set the user defined data read
 *
 * function. This
 * function will be called when one or more characters will be available at the
 * serial input.
 *
 * @param self a pointer to the Uart object.
 * @param callback a user defined function.
 * @param context the user defined pointer that will be passed back to the user
 *                defined function, for user defined purpouses.
 */
void
Uart_setCallback( Uart* self, Uart_Callback* callback, void* context);

/**
 * Retrieves the physical port (unit) to which this object is attached.
 *
 * @param self a pointer to the Uart object.
 * @return the number of the port (unit).
 */
int
Uart_getUnit(Uart const* self);


/**
 * Set bridge between two Uart.
 *
 * @param self a pointer to the Uart object.
 * @return void
 */
void
Uart_setBridge(Uart* first, Uart* second);


/**
 * Enable Rx channel.
 *
 * @param self a pointer to the Uart object.
 * @return void
 */
void
Uart_enableRx(Uart* self);


/**
 * Disable Rx channel.
 *
 * @param self a pointer to the Uart object.
 * @return void
 */
void
Uart_disableRx(Uart* self);


/**
 * Number of byte ready to read inside incoming buffer!
 *
 * @param self a pointer to the Uart object.
 * @return number of byte 2 be read
*/
size_t
Uart_inWaiting(Uart* self);


/**
 * Number of byte ready to be send on Output buffer!
 *
 * @param self a pointer to the Uart object.
 * @return number of byte 2 be read
*/
size_t
Uart_outWaiting(Uart* self);


/**
 * Read status of one othe Uart Connection. 
 *
 * @param self a pointer to the Uart object.
 * @return number of byte 2 be read
*/
Uart_Pin_Status
Uart_getPinStatus(Uart* self , Uart_Hw_Pin pin);



/**
 * Out Buffer Capacity
 *
 * @param self a pointer to the Uart object.
 * @Uart_Hw_Pin  pin 
 * @return bolean
*/
bool
Uart_setPinStatus(Uart* self , Uart_Hw_Pin pin , bool status);

/**
 * out Buffer Size
 *
 * @param self a pointer to the Uart object.
 * @Uart_Hw_Pin  pin 
*/
size_t
Uart_getOutBuffCapacity(Uart* self );


/**
 * In Buffer Capacity
 *
 * @param self a pointer to the Uart object.
 * @Uart_Hw_Pin  pin 
*/
size_t
Uart_getInBuffCapacity(Uart* self );


#ifdef __cplusplus

}

#endif

#endif /* Uart_H */



/******************* (C) COPYRIGHT 2009 MR&D Institute *********END OF FILE****/
