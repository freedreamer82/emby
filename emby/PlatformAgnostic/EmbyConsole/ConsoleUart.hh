#if !defined( UART_CONSOLE_HH)
#define UART_CONSOLE_HH

#include <EmbyLibs/CircularBuffer.hh>
#include <EmbyConsole/Console.hh>
#include <EmbyMachine/Serial.hh>
#include <EmbyLibs/String.hh>


class ConsoleUart :
			public  EmbyConsole::Console

{
	public:
		ConsoleUart( EmbyLog::LogMask mask,EmbyLibs::String const & device,
	    					EmbyMachine::Serial::Serial_Config *config);

	    virtual ~ConsoleUart(){};

    private:
		virtual void transmit(char const* msg, size_t length);

		virtual size_t read(char* buffer, size_t length);

		EmbyMachine::Serial m_uart;
};

#endif
