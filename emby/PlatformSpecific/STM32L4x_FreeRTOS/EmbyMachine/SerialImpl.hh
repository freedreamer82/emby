/**
 * @addtogroup EmbyConsole
 * @{
 * @file EmbyConsole/SerialImpl.h
 *
 * @brief
 * @version 1.0
 * @date 2/02/2012
 *
 */
#if !defined(SERIAL_IMPLH__H)
#define SERIAL_IMPLH__H


/* Includes ------------------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

#include <EmbyThreading/Mutex.hh>
#include <EmbyLibs/CircularBuffer.hh>
extern "C"
{
#include "stm32l4xx.h"

}
/* Exported types ------------------------------------------------------------*/
#define RING_BUFF_SIZE  	16

#define APP_CIRC_BUFF_SIZE  	256
typedef struct SerialImpl SerialImpl;



struct SerialImpl
{
	UART_HandleTypeDef      m_unit;
	EmbyThreading::Mutex 	m_mutex;
	EmbyLibs::CircularBuffer<uint8_t,APP_CIRC_BUFF_SIZE> m_buff;  //app buffer
	uint32_t 			   m_discarded;
	unsigned char		   m_char;
};

/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */




#endif /* UART_ARCH_H */


