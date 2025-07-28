
#include <EmbyMachine/Serial.hh>
#include <EmbyMachine/SerialImpl.hh>

#include <EmbyLibs/CircularBuffer.hh>

#include <EmbyDebug/assert.hh>
#include <EmbyThreading/Thread.hh>
#include <EmbyThreading/ScopedMutex.hh>
#include <EmbyThreading/Worker.hh>
#include <EmbyLibs/StringUtils.hh>


using namespace EmbyExec;
using namespace EmbyLibs;



static std::vector<EmbyMachine::Serial *> g_serials;

extern "C"
{
}


namespace EmbyMachine
{
    Serial::Serial(EmbyLibs::String const &dev, Serial_Config *conf, bool doOpen) :
            m_dev(dev), m_eol(DEFAULT_EOL_CHAR), m_isOpen(false)
    {
		if(m_dev == "uart1")
		{
			m_impl.dev = Uart_Config_Uart_Num_1_dev;
		}
		else if(m_dev == "uart2")
		{
            m_impl.dev = Uart_Config_Uart_Num_2_dev;
		}
		else if(m_dev == "uart3")
		{
            m_impl.dev = Uart_Config_Uart_Num_3_dev;
		}
        else if(m_dev == "uart4")
        {
            m_impl.dev = Uart_Config_Uart_Num_4_dev;
        }
        else if(m_dev == "uart5")
        {
            m_impl.dev = Uart_Config_Uart_Num_5_dev;
        }
		else
		{
			EmbyDebug_ASSERT_FAIL();
		}

        struct termios config;
        if (tcgetattr(fd, &config) != 0) {
            perror("tcgetattr");
            return false;
        }
        config.c_cflag &= ~CSIZE;
        switch (conf->wordLen)
        {
            case Serial::Serial_WordLen::Serial_WordLen_7:
                config.c_cflag |= CS7; /* 7bit x byte */
                break;
            case Serial::Serial_WordLen::Serial_WordLen_8:
                config.c_cflag |= CS8;
                break;
            case Serial::Serial_WordLen::Serial_WordLen_9:
                config.c_cflag |= CS8; // 9bit is not natively supported in Linux, so we use 8bit
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        switch (conf->parity)
        {
            case Serial::Serial_Parity::Serial_Parity_None:
                config.c_cflag &= ~PARENB; /* No parity */
                break;
            case Serial::Serial_Parity::Serial_Parity_Odd:
                config.c_cflag |= PARENB; /* Enable parity */
                config.c_cflag |= PARODD; /* Odd parity */
                config.c_iflag |= INPCK;  /* Input parity check */
                break;
            case Serial::Serial_Parity::Serial_Parity_Even:
                config.c_cflag |= PARENB; /* Enable parity */
                config.c_iflag |= INPCK;  /* Input parity check */
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        switch (conf->stopBits)
        {
            case Serial::Serial_StopBits::Serial_StopBits_0:
                EmbyDebug_ASSERT_FAIL(); // 0 stop bits is not valid
                break;
            case Serial::Serial_StopBits::Serial_StopBits_1:
                config.c_cflag &= ~CSTOPB; /* 1 stop bit */
                break;
            case Serial::Serial_StopBits::Serial_StopBits_2:
                config.c_cflag |= CSTOPB; /* 2 stop bits */
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }
        switch (conf->mode)
        {
            case Serial::Serial_Mode::Serial_Mode_TxRx:
                config.c_cflag |= (CLOCAL | CREAD); /* Enable receiver, ignore modem control lines */
                break;
            case Serial::Serial_Mode::Serial_Mode_Tx:
                config.c_cflag |= (CLOCAL | CREAD); /* Enable transmitter, ignore modem control lines */
                break;
            case Serial::Serial_Mode::Serial_Mode_Rx:
                config.c_cflag &= ~(CLOCAL | CREAD); /* Disable receiver */
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        switch (conf->baudRate)
        {
            case Serial::Serial_BaudRate::Serial_BaudRate_115200:
                cfsetispeed(&config, B115200);
                cfsetospeed(&config, B115200);
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_19200:
                cfsetispeed(&config, B19200);
                cfsetospeed(&config, B19200);
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_4800:
                cfsetispeed(&config, B4800);
                cfsetospeed(&config, B4800);
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_57600:
                cfsetispeed(&config, B57600);
                cfsetospeed(&config, B57600);
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_9600:
                cfsetispeed(&config, B9600);
                cfsetospeed(&config, B9600);
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        switch (conf->flowCtrl)
        {
            case Serial::Serial_FlowCtrl_None:
                config.c_cflag &= ~CRTSCTS;
                break;
            case Serial::Serial_FlowCtrl_RTS_CTS:
                config.c_cflag |= CRTSCTS;
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        // Uart_ctorNopen(&m_impl.m_uart, Uart_Num_1, &m_impl.m_cfg, 0, 0);

        m_impl.m_mutex = EmbyThreading::Mutex();
        m_impl.m_discarded = 0;
        m_impl.m_buff = EmbyLibs::CircularBuffer<uint8_t, APP_CIRC_BUFF_SIZE>();  //app buffer

        if (doOpen)
        {
            open();
        }
    }


    bool Serial::open()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);

        g_serials.push_back((this));

        bool   retval = false;
        struct termios config;
        m_impl.fd = open(m_impl.dev, O_RDWR | O_NONBLOCK);
        if (m_impl.fd >= 0)
        {
            /*Save Old configuration*/
            if (  tcgetattr(m_impl.fd, &config)             ||
                  tcsetattr(m_impl.fd,TCSANOW,&config)      ||
                  tcsetattr(m_impl.fd,TCSAFLUSH,&config)    ||
                  fcntl(m_impl.fd ,F_SETFL, FNDELAY)        || /*read non blocking*/
                  fcntl(m_impl.fd, F_SETFL, O_NONBLOCK) )       /*NON blocking file...*/
            {
                close(m_impl.fd);
                retval = false;
            }
            else
            {
                retval = pthread_create(&m_impl.pThread ,  NULL, Uart_taskRun , self)== 0;
                /*retval = true;*/
                m_impl.devIsOpen = true;
            }
        }
        m_isOpen = true;
        return retval;
        //return true;
    }

    bool Serial::close()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        int retval = close(m_impl.fd);
        m_isOpen = false;
        return retval == 0;
    }

    void
    Serial::sendBreak()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
    }

    size_t
    Serial::write(void const *buffer, size_t length)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        Debug_ASSERT(buffer != NULL);
        return write(m_impl.fd,(uint8_t*)buffer,length);
    }


	static int checkLineInTheBuffer(EmbyMachine::Serial* s )
	{
		size_t len = 0;

		volatile uint32_t i = 0;

		if( ! s->getImpl()->m_buff.isEmpty() )
		{
	//		for(auto it =  s->getImpl()->m_buff.begin() ;
	//			it != s->getImpl()->m_buff.end(); it++)
			for(auto c :  s->getImpl()->m_buff )
			{
				i++;
				EmbyDebug_ASSERT(i <= s->getImpl()->m_buff.getCapacity());
				//volatile auto c = *it;
				if( c == s->getEndofLineChar() )
				{
					len = i;
					break;
				}
			}
		}
		return len;
	}

	static bool readLineInsideBuffer(EmbyLibs::String& line , EmbyMachine::Serial* s)
	{
		EmbyDebug_ASSERT_CHECK_NULL_PTR( s );
		bool retval = false;
		//readline
	//	line.clear();
		int toPop = 0;
		bool dothrow = false;

		EmbySystem_BEGIN_CRITICAL_SECTION;
		if( ! s->getImpl()->m_buff.isEmpty())
		{
			EmbyDebug_ASSERT_CHECK_NULL_PTR(&s->getImpl()->m_buff);

			for(auto c :  s->getImpl()->m_buff )
			{
				toPop++;
				if( c != s->getEndofLineChar() )
				{
					try
					{
						//could throw excep
						line.push_back(c);
					}
					catch (...)
					{
						dothrow = true;
					}
				}
				else
				{
					retval = true;
					break;
				}
			}

			for(int z= 0 ; z < toPop ; z++)
			{
				if(!s->getImpl()->m_buff.isEmpty())
				{
					s->getImpl()->m_buff.pop();
				}
			}
		}
		EmbySystem_END_CRITICAL_SECTION;

		if(dothrow)
		{
			throw std::bad_alloc();
		}

		return retval;
	}

    EmbyLibs::String Serial::readline(int32_t timeoutMs)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);

        volatile bool exit = false;
        volatile size_t len = 0;
        EmbyLibs::String line;
        line.clear();

        uint64_t start = EmbySystem::upTimeMs();

        do
        {
            EmbySystem_BEGIN_CRITICAL_SECTION;
            {
                len = checkLineInTheBuffer(this);
            }
            EmbySystem_END_CRITICAL_SECTION;

            if (len != 0)
            {
                line.reserve(len);
                if (line.data())
                {
                    //is valid the string
                    exit = readLineInsideBuffer(line, this);
                }
                if (noEmptyString)
                {
                    EmbyLibs::trim(line);
                    exit = !line.empty();
                }
            }
            else
            {
                //EmbyThreading::Thread::yield();
                EmbySystem::delayMs(3);
            }

            if (timeoutMs > 0)
            {
                if ((EmbySystem::upTimeMs() - start) > timeoutMs)
                {
//					 throw EmbyLibs::Timeout("Serial");
                    exit = true;
                }
            }

        } while (!exit);

        return line;
    }

    size_t Serial::read(void *buffer, size_t length)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);

        size_t retval = 0;
        uint8_t *data = (uint8_t *) buffer;

        if (buffer)
        {
            size_t i = 0;
            while (!m_impl.m_buff.isEmpty() && i < length)
            {
                {
                    uint8_t byte ;
                    if (m_impl.m_buff.front(byte))
                    {
                        data[i] = byte;
                    }
                    if (!m_impl.m_buff.isEmpty())
                    {
                        m_impl.m_buff.pop();
                    }
                }
                ++i;
            }

            retval = i;
        }
        return retval;
    }

    void Serial::enableRx()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);


    }


    void Serial::flush()
    {
        EmbySystem_BEGIN_CRITICAL_SECTION;
        {
            m_impl.m_buff.clear();
        }
        EmbySystem_END_CRITICAL_SECTION;
    }


    void Serial::disableRx()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
    }

    Serial::Serial_Pin_Status Serial::getPinStatus(Serial_Hw_Pin pin)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        EmbyDebug_ASSERT_FAIL();
        return Serial::Serial_Pin_Status::Serial_Pin_Unkwown;
    }


    bool Serial::setPinStatus(Serial_Hw_Pin pin, bool status)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        EmbyDebug_ASSERT_FAIL();
        return false;
    }

    int Serial::getAvailableBytes()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        int bytes = 0;

        EmbySystem_BEGIN_CRITICAL_SECTION;
        {
            bytes = m_impl.m_buff.getSize();
        }
        EmbySystem_END_CRITICAL_SECTION;

        return bytes;
    }
}

