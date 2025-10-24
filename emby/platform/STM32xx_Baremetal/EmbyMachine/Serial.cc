#include <EmbyMachine/Serial.hh>
#include <EmbyMachine/SerialImpl.hh>


#include <EmbyLibs/CircularBuffer.hh>

#include <EmbyThreading/Flags.hh>
#include <EmbyDebug/assert.hh>
#include <EmbyThreading/Thread.hh>
#include <EmbyThreading/ScopedMutex.hh>
#include <EmbyThreading/Worker.hh>
#include <EmbyLibs/StringUtils.hh>

#include <EmbyLog/Log.hh>

EmbyLog_MODULE_LOG_CLASS("s");

using namespace EmbyExec;
using namespace EmbyLibs;

#define CR1_SBK_Set               ((uint16_t)0x0001)  /*!< USART Break Character send Mask */

extern "C"
{
//this is a weak function if return true the character is consumed and not push to the internal buffer
__attribute__((weak)) bool Serial_charIsrCallback(EmbyLibs::String const &dev, char character)
{
    return false;
} ;
__attribute__((weak)) void BSP_EMBY_UART_INIT(UART_HandleTypeDef* huart)
{
    return;
} ;

}

static void disableIRQ(UART_HandleTypeDef *base)
{
//        BSP_UART_RTS_PIN(base, true);
#ifdef EMBY_CFG_SERIAL_ENABLE_LPUART1
    if (base->Instance == LPUART1)
    {
        NVIC_DisableIRQ(LPUART1_IRQn);
    }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_USART1
    if (base->Instance == USART1)
    {
        NVIC_DisableIRQ(USART1_IRQn);
    }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_USART2
    if (base->Instance == USART2)
    {
        NVIC_DisableIRQ(USART2_IRQn);
    }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_USART3
    if (base->Instance == USART3)
    {
        NVIC_DisableIRQ(USART3_IRQn);
    }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_UART4
    if (base->Instance == UART4 )
    {
      NVIC_DisableIRQ(UART4_IRQn);
    }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_UART5
    if (base->Instance == UART5 )
    {
        NVIC_DisableIRQ(UART5_IRQn);
    }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_UART7
    if (base->Instance == UART7)
    {
        NVIC_DisableIRQ(UART7_IRQn);
    }
#endif
//    else
//    {
//        EmbyDebug_ASSERT_FAIL();
//    }
}

static void enableIRQ(UART_HandleTypeDef *base)
{
#ifdef EMBY_CFG_SERIAL_ENABLE_LPUART1
    if (base->Instance == LPUART1)
    {
        NVIC_EnableIRQ(LPUART1_IRQn);
    }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_USART1
    if (base->Instance == USART1)
    {
        NVIC_EnableIRQ(USART1_IRQn);
    }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_USART2
    if (base->Instance == USART2)
    {
        NVIC_EnableIRQ(USART2_IRQn);
    }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_USART3
    if (base->Instance == USART3)
    {
        NVIC_EnableIRQ(USART3_IRQn);
    }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_UART4
    if (base->Instance == UART4 )
    {
      NVIC_EnableIRQ(UART4_IRQn);
    }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_UART5
    if (base->Instance == UART5 )
    {
        NVIC_EnableIRQ(UART5_IRQn);
    }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_UART7
    if (base->Instance == UART7)
    {
        NVIC_EnableIRQ(UART7_IRQn);
    }
#endif

//        BSP_UART_RTS_PIN(base, false);
}


uint32_t USART_IsActiveFlag_RXNE(USART_TypeDef *USARTx)
{

#if defined(STM32H7xx)
    return (READ_BIT(USARTx->ISR, USART_ISR_RXNE_RXFNE) == (USART_ISR_RXNE_RXFNE));
#else
    return (READ_BIT(USARTx->ISR, USART_ISR_RXNE) == (USART_ISR_RXNE));
#endif /* STM32H7xx */

}

uint8_t USART_ReceiveData8(USART_TypeDef *USARTx)
{
    return (uint8_t) (READ_BIT(USARTx->RDR, USART_RDR_RDR));
}

void USART_ClearFlag_ORE(USART_TypeDef *USARTx)
{
    WRITE_REG(USARTx->ICR, USART_ICR_ORECF);
}

void USART_ClearFlag_Error(USART_TypeDef *USARTx)
{
    WRITE_REG(USARTx->ICR, 0xFFFFFFFF);
}

void USART_EnableIT_RXNE(USART_TypeDef *USARTx)
{
    SET_BIT(USARTx->CR1, USART_CR1_RXNEIE);
}

//void LL_USART_Enable(USART_TypeDef *USARTx)
//{
//   SET_BIT(USARTx->CR1, USART_CR1_UE);
//}

static std::vector<EmbyMachine::Serial *> g_serials;

static EmbyMachine::Serial *getSerialFromUnit(USART_TypeDef *base)
{
    for (EmbyMachine::Serial *s: g_serials)
    {
        if (s->getImpl()->m_unit.Instance == base)
        {
            return s;
        }
    }
    return nullptr;
}


extern "C"
{
//	void doHandleIRQ()
//	{
//		for (EmbyMachine::Serial *s: g_serials)
//		{
//			if (s && s->getImpl()->m_unit.Instance)
//			{
//				if (USART_IsActiveFlag_RXNE(s->getImpl()->m_unit.Instance))
//				{
//					uint8_t c = USART_ReceiveData8(s->getImpl()->m_unit.Instance);
//					if (!Serial_charIsrCallback(s->getDevice(), c))
//					{
//						if (!s->getImpl()->m_buff.isFull())
//						{
//							s->getImpl()->m_buff.push(c);
//						}
//						else
//						{
//							s->getImpl()->m_discarded++;
//							__NOP();
//						}
//					}
//				}
//				USART_ClearFlag_ORE(s->getImpl()->m_unit.Instance);
//			}
//		}
//
//	}

void doHandleIRQ()
{
    for (EmbyMachine::Serial *s: g_serials)
    {
        if (s && s->getImpl()->m_unit.Instance)
        {
            if (__HAL_USART_GET_FLAG(&s->getImpl()->m_unit, USART_FLAG_RXNE) == SET)
            {
                uint8_t c = LL_USART_ReceiveData8(s->getImpl()->m_unit.Instance);
                if (!Serial_charIsrCallback(s->getDevice(), c))
                {
                    if (!s->getImpl()->m_buff.isFull())
                    {
                        s->getImpl()->m_buff.push(c);
                    }
                    else
                    {
                        s->getImpl()->m_discarded++;
                        __NOP();
                    }
                }
            }
            USART_ClearFlag_ORE(s->getImpl()->m_unit.Instance);
        }
    }

}
void LPUART1_IRQHandler(void)
{
    doHandleIRQ();
}
void USART1_IRQHandler(void)
{
    doHandleIRQ();
}

void USART3_4_IRQHandler(void)
{
    doHandleIRQ();
}

void USART3_IRQHandler(void)
{
    doHandleIRQ();
}

void UART4_IRQHandler(void)
{
    doHandleIRQ();
}

void USART2_IRQHandler(void)
{
    doHandleIRQ();
}

void UART5_IRQHandler(void)
{
    doHandleIRQ();
}

void UART7_IRQHandler(void)
{
    doHandleIRQ();
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    //clear flags??
    EmbyMachine::Serial *serial = getSerialFromUnit((USART_TypeDef *) huart->Instance);
    if (serial)
    {
        HAL_UART_Receive_IT(&serial->getImpl()->m_unit, &serial->getImpl()->m_char, 1);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    EmbyMachine::Serial *serial = getSerialFromUnit((USART_TypeDef *) huart->Instance);
    if (serial)
    {
        if (!serial->getImpl()->m_buff.isFull())
        {
            serial->getImpl()->m_buff.push(serial->getImpl()->m_char);
        }
        else
        {
            serial->getImpl()->m_discarded++;
        }
        HAL_UART_Receive_IT(&serial->getImpl()->m_unit, &serial->getImpl()->m_char, 1);
    }

}
}//end extern


namespace EmbyMachine
{
    Serial::Serial(EmbyLibs::String const &dev, Serial_Config *conf, bool doOpen) :
            m_dev(dev), m_eol(DEFAULT_EOL_CHAR), m_isOpen(false)
    {
#ifdef EMBY_CFG_SERIAL_ENABLE_LPUART1
        if (m_dev == "lpuart1")
        {
            m_impl.m_unit.Instance = LPUART1;
        }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_USART1
        if (m_dev == "uart1")
        {
            m_impl.m_unit.Instance = USART1;
        }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_USART2
        if (m_dev == "uart2")
        {
            m_impl.m_unit.Instance = USART2;
        }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_USART3
        if (m_dev == "uart3")
        {
            m_impl.m_unit.Instance = USART3;
        }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_UART4
        if (m_dev == "uart4")
        {
          m_impl.m_unit.Instance = UART4;
        }
#endif

#ifdef EMBY_CFG_SERIAL_ENABLE_UART5
        if (m_dev == "uart5")
        {
            m_impl.m_unit.Instance = UART5;
        }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_UART7
        if (m_dev == "uart7")
        {
            m_impl.m_unit.Instance = UART7;
        }
#endif
        /*	else if(m_dev == "uart4")
            {
                m_impl.m_unit = USART4;
            }
            else if(m_dev == "uart4")
            {
                m_impl.m_unit = UART5;
            }
            else if(m_dev == "uart5")
            {
                m_impl.m_unit = UART5;
            }*/
        EmbyDebug_ASSERT(m_impl.m_unit.Instance != nullptr);
        m_impl.m_unit.Init.WordLength = UART_WORDLENGTH_8B;
        m_impl.m_unit.Init.StopBits = UART_STOPBITS_1;
        m_impl.m_unit.Init.Parity = UART_PARITY_NONE;
        m_impl.m_unit.Init.Mode = UART_MODE_TX_RX;
        m_impl.m_unit.Init.OverSampling = UART_OVERSAMPLING_16;
        m_impl.m_unit.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
        m_impl.m_unit.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
        m_impl.m_unit.gState = HAL_UART_STATE_RESET;

        BSP_EMBY_UART_INIT(&m_impl.m_unit);

        switch (conf->baudRate)
        {
            case Serial::Serial_BaudRate::Serial_BaudRate_115200:
                m_impl.m_unit.Init.BaudRate = 115200;
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_19200:
                m_impl.m_unit.Init.BaudRate = 19200;
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_74880:
                m_impl.m_unit.Init.BaudRate = 74880;
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_4800:
                m_impl.m_unit.Init.BaudRate = 4800;
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_57600:
                m_impl.m_unit.Init.BaudRate = 57600;
                break;
            case Serial::Serial_BaudRate::Serial_BaudRate_9600:
                m_impl.m_unit.Init.BaudRate = 9600;
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        switch (conf->flowCtrl)
        {
            case Serial::Serial_FlowCtrl_None:
                m_impl.m_unit.Init.HwFlowCtl = UART_HWCONTROL_NONE;
                break;
            case Serial::Serial_FlowCtrl_RTS_CTS:
                m_impl.m_unit.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
                break;
            default:
                EmbyDebug_ASSERT_FAIL();
                break;
        }

        auto retval = HAL_UART_Init(&m_impl.m_unit);
        EmbyDebug_ASSERT(retval == HAL_OK);
        //	LL_USART_Disable( m_impl.m_unit.Instance );


        __HAL_UART_DISABLE_IT(&m_impl.m_unit, UART_IT_TXE | UART_IT_RXNE);

        m_impl.m_mutex = EmbyThreading::Mutex();
        m_impl.m_discarded = 0;
        m_impl.m_buff = EmbyLibs::CircularBuffer<uint8_t, APP_CIRC_BUFF_SIZE>();  //app buffer

        close();

        if (doOpen)
        {
            open();
        }
    }


    bool Serial::open()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);

        g_serials.push_back((this));
#ifdef EMBY_CFG_SERIAL_ENABLE_LPUART1
        if (m_impl.m_unit.Instance == LPUART1)
        {
            HAL_NVIC_SetPriority(LPUART1_IRQn, 2, 1);
            HAL_NVIC_EnableIRQ(LPUART1_IRQn);
        }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_USART1
        if (m_impl.m_unit.Instance == USART1)
        {
            HAL_NVIC_SetPriority(USART1_IRQn, 2, 1);
            HAL_NVIC_EnableIRQ(USART1_IRQn);
        }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_USART2
        if (m_impl.m_unit.Instance == USART2)
        {
            HAL_NVIC_SetPriority(USART2_IRQn, 2, 1);
            HAL_NVIC_EnableIRQ(USART2_IRQn);
        }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_USART3
        if (m_impl.m_unit.Instance == USART3)
        {
            HAL_NVIC_SetPriority(USART3_IRQn, 2, 1);
            HAL_NVIC_EnableIRQ(USART3_IRQn);
        }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_UART4
        if (m_impl.m_unit.Instance == UART4)
        {
          HAL_NVIC_SetPriority(UART4_IRQn, 2, 1);
          HAL_NVIC_EnableIRQ(UART4_IRQn);
        }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_UART5
        if (m_impl.m_unit.Instance == UART5)
        {
                HAL_NVIC_SetPriority(UART5_IRQn, 2, 1);
                HAL_NVIC_EnableIRQ(UART5_IRQn);
        }
#endif
#ifdef EMBY_CFG_SERIAL_ENABLE_UART7
        if (m_impl.m_unit.Instance == UART7)
        {
            HAL_NVIC_SetPriority(UART7_IRQn, 2, 1);
            HAL_NVIC_EnableIRQ(UART7_IRQn);
        }
#endif
        USART_EnableIT_RXNE(m_impl.m_unit.Instance);

        m_isOpen = true;
        return true;
    }


    bool Serial::close()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);

        __HAL_UART_DISABLE_IT(&m_impl.m_unit, UART_IT_TXE | UART_IT_RXNE);
        m_isOpen = false;
        return true;
    }

    void
    Serial::sendBreak()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        __HAL_UART_SEND_REQ(&m_impl.m_unit, UART_SENDBREAK_REQUEST);
    }

    size_t
    Serial::write(void const *buffer, size_t length)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        HAL_UART_Transmit(&m_impl.m_unit, (uint8_t *) buffer, length, 1000);
        return length;
    }

    static int checkLineInTheBuffer(EmbyMachine::Serial *s)
    {
        int len = -1;

        volatile uint32_t i = 0;

        if (!s->getImpl()->m_buff.isEmpty())
        {
            for (auto c: s->getImpl()->m_buff)
            {
                i++;
                EmbyDebug_ASSERT(i <= s->getImpl()->m_buff.getCapacity());
                if (c == s->getEndofLineChar())
                {
                    len = i;
                    break;
                }
            }
        }
        return len;
    }

    static bool readLineInsideBuffer(EmbyLibs::String &line, EmbyMachine::Serial *s)
    {
        EmbyDebug_ASSERT_CHECK_NULL_PTR(s);
        bool retval = false;

        line.clear();
        int toPop = 0;
        bool empty = false;
        disableIRQ(&s->getImpl()->m_unit);
        empty = s->getImpl()->m_buff.isEmpty();
        enableIRQ(&s->getImpl()->m_unit);

        if (!empty)
        {
            disableIRQ(&s->getImpl()->m_unit);

            EmbyDebug_ASSERT_CHECK_NULL_PTR(&s->getImpl()->m_buff);

            for (auto c: s->getImpl()->m_buff)
            {
                toPop++;
                if (c != s->getEndofLineChar())
                {
                    auto err = line.push_back(c);
                    if (err < 0)
                    {
                        toPop = 0;
                        retval = false;
                        break;
                    }
                }
                else
                {
                    retval = true;
                    break;
                }
            }
            enableIRQ(&s->getImpl()->m_unit);

            disableIRQ(&s->getImpl()->m_unit);
            for (int z = 0; z < toPop; z++)
            {
                if (!s->getImpl()->m_buff.isEmpty())
                {
                    s->getImpl()->m_buff.pop();
                }
            }
            enableIRQ(&s->getImpl()->m_unit);
        }


        return retval;
    }

    EmbyLibs::String Serial::readline(int32_t timeoutMs)
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);

        volatile bool exit = false;
        volatile size_t len = 0;
        String line;
        line.clear();

        uint64_t start = EmbySystem::upTimeMs();

        do
        {
            disableIRQ(&getImpl()->m_unit);
            {
                len = checkLineInTheBuffer(this);
            }
            enableIRQ(&getImpl()->m_unit);

            if (len >= 0)
            {

                auto err = line.reserve(len);
                if (err < 0)
                {
                    line.clear();
                    return line;
                }
                exit = readLineInsideBuffer(line, this);
            }
            else
            {
                EmbySystem::delayMs(5);
            }

            if (timeoutMs > 0)
            {
                if ((EmbySystem::upTimeMs() - start) > timeoutMs)
                {
                    line.clear();
                    return line;
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
            disableIRQ(&getImpl()->m_unit);

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
            enableIRQ(&getImpl()->m_unit);

        }
        return retval;
    }

    void Serial::enableRx()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        __HAL_UART_ENABLE_IT(&m_impl.m_unit, UART_IT_RXNE);

    }


    void Serial::flush()
    {
        disableIRQ(&getImpl()->m_unit);;
        {
            m_impl.m_buff.clear();
        }
        enableIRQ(&getImpl()->m_unit);;
    }


    void Serial::disableRx()
    {
        EmbyThreading::ScopedMutex(m_impl.m_mutex);
        __HAL_UART_DISABLE_IT(&m_impl.m_unit, UART_IT_RXNE);
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

        disableIRQ(&getImpl()->m_unit);
        {
            bytes = m_impl.m_buff.getSize();
        }
        enableIRQ(&getImpl()->m_unit);

        return bytes;
    }
}