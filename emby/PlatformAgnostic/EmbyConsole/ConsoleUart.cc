#include "ConsoleUart.hh"
#include "EmbySystem/System.hh"

ConsoleUart::ConsoleUart(EmbyLog::LogMask mask,EmbyLibs::String const & device,
		EmbyMachine::Serial::Serial_Config *config) :
				 Console(mask) , m_uart(device,config)
{
	m_uart.open();
}

void ConsoleUart::transmit(char const* msg, size_t length)
{
	m_uart.write(msg,length);
}

size_t ConsoleUart::read(char* buffer, size_t length)
{
	return m_uart.read(buffer,length);
}
