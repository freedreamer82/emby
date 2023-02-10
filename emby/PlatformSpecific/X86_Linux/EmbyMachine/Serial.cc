
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
//		if(m_dev == "uart1")
//		{
//			m_impl.m_unit.Instance = USART1;
//		}
//		else if(m_dev == "uart2")
//		{
//			m_impl.m_unit.Instance = USART2;
//		}
//		else if(m_dev == "uart3")
//		{
//			m_impl.m_unit.Instance = USART3;
//		}
        /*	else if(m_dev == "uart4")
            {
                m_impl.m_unit = USART4;
            }
            else if(m_dev == "uart4")
            {
                m_impl.m_unit = USART5;
            }
            else if(m_dev == "uart5")
            {
                m_impl.m_unit = UART5;
            }*/
//		else
//		{
//			EmbyDebug_ASSERT_FAIL();
//		}

//        m_impl.m_unit.Init.WordLength = UART_WORDLENGTH_8B;
//        m_impl.m_unit.Init.StopBits = UART_STOPBITS_1;
//        m_impl.m_unit.Init.Parity = UART_PARITY_NONE;
//        m_impl.m_unit.Init.Mode = UART_MODE_TX_RX;
//        m_impl.m_unit.Init.OverSampling = UART_OVERSAMPLING_16;
//        m_impl.m_unit.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
//        m_impl.m_unit.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
//        m_impl.m_unit.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
//        m_impl.m_unit.gState = HAL_UART_STATE_RESET;

        switch (conf->baudRate)
        {
            case Serial::Serial_BaudRate::Serial_BaudRate_115200:
//                m_impl.m_unit.Init.BaudRate = 115200;
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_19200:
//                m_impl.m_unit.Init.BaudRate = 19200;
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_4800:
//                m_impl.m_unit.Init.BaudRate = 4800;
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_57600:
//                m_impl.m_unit.Init.BaudRate = 57600;
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_9600:
//                m_impl.m_unit.Init.BaudRate = 9600;
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        switch (conf->flowCtrl)
        {
            case Serial::Serial_FlowCtrl_None:
//                m_impl.m_unit.Init.HwFlowCtl = UART_HWCONTROL_NONE;
                break;
            case Serial::Serial_FlowCtrl_RTS_CTS:
//                m_impl.m_unit.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
                break;
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        Uart_ctorNopen(&m_impl.m_uart, Uart_Num_1, &m_impl.m_cfg, 0, 0);

        m_impl.m_mutex = EmbyThreading::Mutex();
        m_impl.m_discarded = 0;
//        m_impl.m_buff = EmbyLibs::CircularBuffer<uint8_t, APP_CIRC_BUFF_SIZE>();  //app buffer

        if (doOpen)
        {
            open();
        }
    }


    bool Serial::open()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);

        g_serials.push_back((this));

        m_isOpen = true;
        return true;
    }


    bool Serial::close()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);


        m_isOpen = false;
        return true;
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

        return length;
    }


//	static int checkLineInTheBuffer(EmbyMachine::Serial* s )
//	{
//		size_t len = 0;
//
//		volatile uint32_t i = 0;
//
//		if( ! s->getImpl()->m_buff.isEmpty() )
//		{
//	//		for(auto it =  s->getImpl()->m_buff.begin() ;
//	//			it != s->getImpl()->m_buff.end(); it++)
//			for(auto c :  s->getImpl()->m_buff )
//			{
//				i++;
//				EmbyDebug_ASSERT(i <= s->getImpl()->m_buff.getCapacity());
//				//volatile auto c = *it;
//				if( c == s->getEndofLineChar() )
//				{
//					len = i;
//					break;
//				}
//			}
//		}
//		return len;
//	}
//
//	static bool readLineInsideBuffer(EmbyLibs::String& line , EmbyMachine::Serial* s)
//	{
//		EmbyDebug_ASSERT_CHECK_NULL_PTR( s );
//		bool retval = false;
//		//readline
//	//	line.clear();
//		int toPop = 0;
//		bool dothrow = false;
//
//		EmbySystem_BEGIN_CRITICAL_SECTION;
//		if( ! s->getImpl()->m_buff.isEmpty())
//		{
//			EmbyDebug_ASSERT_CHECK_NULL_PTR(&s->getImpl()->m_buff);
//
//			for(auto c :  s->getImpl()->m_buff )
//			{
//				toPop++;
//				if( c != s->getEndofLineChar() )
//				{
//					try
//					{
//						//could throw excep
//						line.push_back(c);
//					}
//					catch (...)
//					{
//						dothrow = true;
//					}
//				}
//				else
//				{
//					retval = true;
//					break;
//				}
//			}
//
//			for(int z= 0 ; z < toPop ; z++)
//			{
//				if(!s->getImpl()->m_buff.isEmpty())
//				{
//					s->getImpl()->m_buff.pop();
//				}
//			}
//		}
//		EmbySystem_END_CRITICAL_SECTION;
//
//		if(dothrow)
//		{
//			throw std::bad_alloc();
//		}
//
//		return retval;
//	}

    EmbyLibs::String Serial::readline(int32_t timeoutMs)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);

//        volatile bool exit = false;
//        volatile size_t len = 0;
        EmbyLibs::String line;
//        line.clear();
//
//        uint64_t start = EmbySystem::upTimeMs();
//
//        do
//        {
//            EmbySystem_BEGIN_CRITICAL_SECTION;
//            {
//                len = checkLineInTheBuffer(this);
//            }
//            EmbySystem_END_CRITICAL_SECTION;
//
//            if (len != 0)
//            {
//                line.reserve(len);
//                if (line.data())
//                {
//                    //is valid the string
//                    exit = readLineInsideBuffer(line, this);
//                }
//                if (noEmptyString)
//                {
//                    EmbyLibs::trim(line);
//                    exit = !line.empty();
//                }
//            }
//            else
//            {
//                //EmbyThreading::Thread::yield();
//                EmbySystem::delayMs(3);
//            }
//
//            if (timeoutMs > 0)
//            {
//                if ((EmbySystem::upTimeMs() - start) > timeoutMs)
//                {
////					 throw EmbyLibs::Timeout("Serial");
//                    exit = true;
//                }
//            }
//
//        } while (!exit);

        return line;
    }

    size_t Serial::read(void *buffer, size_t length)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
//
//		size_t retval = 0;
//		uint8_t * data = (uint8_t *)buffer;
//
//		if(buffer)
//		{
//			size_t i=0;
//			EmbySystem_BEGIN_CRITICAL_SECTION;
//
//			while( !m_impl.m_buff.isEmpty() && i < length )
//			{
//
//				{
//					data[i] = (uint8_t) m_impl.m_buff.front();
//					if(! m_impl.m_buff.isEmpty())
//					{
//						m_impl.m_buff.pop();
//					}
//				}
//				++i;
//			}
//
//			EmbySystem_END_CRITICAL_SECTION;
//			retval = i;
//		}
//		return retval;
        return 0;
    }

    void Serial::enableRx()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);


    }


    void Serial::flush()
    {
//        EmbySystem_BEGIN_CRITICAL_SECTION;
//        {
//            m_impl.m_buff.clear();
//        }
//        EmbySystem_END_CRITICAL_SECTION;
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

//        EmbySystem_BEGIN_CRITICAL_SECTION;
//        {
//            bytes = m_impl.m_buff.getSize();
//        }
//        EmbySystem_END_CRITICAL_SECTION;

        return bytes;
    }
}

