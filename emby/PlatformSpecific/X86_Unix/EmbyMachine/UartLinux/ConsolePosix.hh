
/**
 * @addtogroup Application
 * @{
 *
 * @file Console.h
 *
 * @brief
 *
 * @author Marco Garzola
 * @version 00.01
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/


#if !defined( EMBYCONSOLE_POSIXCONSOLE_HH )
#define EMBYCONSOLE_POSIXCONSOLE_HH

/* Includes ------------------------------------------------------------------*/
//#include "SysLog.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <EmbyThreading/Mutex.hh>
#include <stdarg.h>
#include <EmbyLibs/BitMask.hh>
#include <EmbyConsole/Console.hh>
#include <EmbyDebug/assert.hh>
#include <EmbyLibs/dynarray.hh>
#include <EmbyMachine/UartLinux/Uart.h>
#include <EmbyLog/LogLevel.hh>
/* Exported types ------------------------------------------------------------*/
namespace EmbyConsole
{
	class ConsoleTelnet :
			public Console
	{

	public:

		ConsoleTelnet(EmbyLog::LogMask mask) : Console(mask)
		{
			Uart_Config cfg;
			cfg.baudRate = 115200;
			cfg.flowCtrl = Uart_FlowCtrl_None;
			cfg.mode = Uart_Mode_TxRx;
			cfg.stopBits = Uart_StopBits_1;
			cfg.wordLen = Uart_WordLen_8;
			cfg.parity = Uart_Parity_None;
			Uart_ctorNopen(&m_uart,Uart_Num_1 ,&cfg,0,0);
		}

		virtual void transmit(char const* msg, size_t length)
		{
			Uart_write(&m_uart,msg, length);
		}

	    virtual size_t read(char* buffer, size_t length)
	    {
	    	return Uart_read(&m_uart,buffer,length);
	    }

		private:
			char const  * m_dev;
			Uart  m_uart;
	};

}

#endif
