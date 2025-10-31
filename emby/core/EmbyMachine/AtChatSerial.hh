#if !defined( AT_CHAT_SERIAL_HH)
#define AT_CHAT_SERIAL_HH

#include "AtChat.hh"
#include <EmbyMachine/Serial.hh>
#include <EmbyThreading/Thread.hh>
#include <EmbyThreading/ScopedMutex.hh>

using namespace EmbyLibs;
using namespace EmbyMachine;

class AtChatSerial : public AtChat
{
    public:
    AtChatSerial(EmbyLibs::String const &dev, const char *eol, Serial::Serial_Config *config) :
            AtChat(eol, false), m_serial(dev, config)
    {
        m_serial.open();
        if (m_thread)
        {
            m_thread->start();
        }
        flush();
    }

		virtual ~AtChatSerial() = default;

		virtual size_t
		write( void const* buffer, size_t length )
		{
			EmbyThreading::ScopedMutex sm(m_mutex);
			return m_serial.write(buffer,length);
		}

        virtual EmbyLibs::String  readline( int32_t timeoutMs = 0,bool noEmptyString = false)
        {
        	EmbyThreading::ScopedMutex sm(m_mutex);
        	return m_serial.readline(timeoutMs,noEmptyString);
        }

        virtual bool isDeviceOpen()
		{
        	EmbyThreading::ScopedMutex sm(m_mutex);
        	return m_serial.isOpen();
		}

        virtual size_t
		read( void* buffer , size_t bufferSize)
        {
        	EmbyThreading::ScopedMutex sm(m_mutex);
        	return  m_serial.read(buffer,bufferSize);
        }

        EmbyMachine::Serial &
        getUart()
        {
        	return m_serial;
        }

        void flush()
        {
        	EmbyThreading::ScopedMutex sm(m_mutex);
        	m_serial.flush();
        	m_mode = AtChat_Mode::AtChat_AT_MODE_IDLE;
        }

    private:
        EmbyMachine::Serial 		m_serial;
};

#endif
