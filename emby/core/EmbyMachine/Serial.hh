/**
 * @file EmbyConsole/Serial.h
 *
 * @brief <brief description>
 *
 * <detailed description, on one or more rows>
 *
 * @author Garzola Marco
 * @version <00.01>
 * @date 11/11/2014
 * @note None
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/

#if !defined(EMBY_SERIAL__H)
#define EMBY_SERIAL__H


/* Includes ------------------------------------------------------------------*/

#include "SerialImpl.hh"
#include <EmbyExec/ListenersHandler.hh>
#include <EmbyLibs/String.hh>

/* Exported types ------------------------------------------------------------*/

namespace EmbyMachine
{
    typedef class Serial Serial;

    //Serial is a Listener Handler of Listener of Serial
//	class Serial : public EmbyExec::ListenersHandler<EmbyExec::Listener<Serial>*>
    class Serial
    {
    public:

        static constexpr char DEFAULT_EOL_CHAR = '\n';

        enum class Serial_Num : uint8_t
        {
            Serial_Num_1,
            Serial_Num_2,
            Serial_Num_3,
            Serial_Num_4,
            Serial_Num_5,
            Serial_Num_6,
            Serial_Num_7,
            Serial_Num_8,
            Serial_Num_Unkwown
        };

        enum class Serial_WordLen : uint8_t
        {
            Serial_WordLen_7 = 7,
            Serial_WordLen_8,
            Serial_WordLen_9
        };

        enum class Serial_StopBits : uint8_t
        {
            Serial_StopBits_0,
            Serial_StopBits_1,
            Serial_StopBits_2
        };

        enum class Serial_Parity : uint8_t
        {
            Serial_Parity_None,
            Serial_Parity_Odd,
            Serial_Parity_Even
        };

        enum Serial_FlowCtrl : uint8_t
        {
            Serial_FlowCtrl_None,
            Serial_FlowCtrl_RTS_CTS,
            Serial_FlowCtrl_RTS,
            Serial_FlowCtrl_CTS,
            Serial_FlowCtrl_SW_RTS_CTS,
            Serial_FlowCtrl_SW_RTS,
            Serial_FlowCtrl_SW_CTS,
        };

        enum class Serial_Mode : uint8_t
        {
            Serial_Mode_TxRx,
            Serial_Mode_Tx,
            Serial_Mode_Rx
        };

        enum class Serial_Err : uint8_t
        {
            Serial_Err_None,
            Serial_Err_Config_FlowCtrl,
            Serial_Err_Config_WorldLen,
            Serial_Err_Config_Parity,
            Serial_Err_Config_StopBits,
            Serial_Err_Config_BaudRate,
            Serial_Err_Config_Mode,
            Serial_Err_Unknown
        };

        enum class Serial_BaudRate : uint8_t
        {
            Serial_BaudRate_115200,
            Serial_BaudRate_74880,
            Serial_BaudRate_57600,
            Serial_BaudRate_19200,
            Serial_BaudRate_9600,
            Serial_BaudRate_4800,
            Serial_Err_Unknown
        };


        typedef struct
        {
            Serial_WordLen wordLen;
            Serial_Parity parity;
            Serial_StopBits stopBits;
            Serial_FlowCtrl flowCtrl;
            Serial_BaudRate baudRate;
            Serial_Mode mode;
            uint16_t buffSize;  //not all machine can provide this
        }
                Serial_Config;

        enum class Serial_Pin_Status : uint8_t
        {
            Serial_Pin_Low,
            Serial_Pin_High,
            Serial_Pin_Unkwown,
            Serial_Pin_NotConnected
        };


        enum class Serial_Hw_Pin : uint8_t
        {
            Serial_Hw_Pin_CTS,
            Serial_Hw_Pin_RTS,
            Serial_Hw_Pin_DTR,
            Serial_Hw_Pin_DCD,
            Serial_Hw_Pin_RING,
            Serial_Hw_Pin_TX,
            Serial_Hw_Pin_RX,
            Serial_Hw_Pin_DSR
        };

        Serial(EmbyLibs::String const &m_dev, Serial_Config *config, bool open = true);

        EmbyLibs::String &getDevice()
        {
            return m_dev;
        }

        bool open();

        bool isOpen()
        {
            return m_isOpen;
        }

        bool close();

        size_t
        write(void const *buffer, size_t length);

        void
        write(char const *text)
        {
            write(text, strlen(text));
        }

        void write(EmbyLibs::String const &text)
        {
            write(text.data(), text.length());
        }


        /**
         * Reads From the Uart Buffer up to bufferSize bytes.
         * Returns the actually read bytes. This function never blocks the caller:
         * if no data are available 0 is returned
         *
         * @param buffer a pointer to the sequence of characters to read.
         * @param bufferSize max size of the bytes to read.
         */
        size_t
        read(void *buffer, size_t bufferSize);


        /*
         * readline return 0 if found , < 0 otherwise.
         * timeoutMs = time to wait line , if negative wait forever.
         * */
        EmbyLibs::String readline(int32_t timeoutMs = 0);

        /**
         * Enable Rx channel.
         *
         * @param self a pointer to the Uart object.
         * @return void
         */
        void
        enableRx();

        /**
         * Disable Rx channel.
         *
         * @param self a pointer to the Uart object.
         * @return void
         */
        void
        disableRx();

        /**
         * Read status of one othe Uart Connection.
         *
         * @param self a pointer to the Uart object.
         * @return number of byte 2 be read
        */
        Serial_Pin_Status
        getPinStatus(Serial_Hw_Pin pin);


        /**
         * Out Buffer Capacity
         *
         * @param self a pointer to the Uart object.
         * @Serial_Hw_Pin  pin
         * @return bolean
        */
        bool
        setPinStatus(Serial_Hw_Pin pin, bool status);


        void
        sendBreak();


        /*
         * get Available bytes to be read
         * */
        int
        getAvailableBytes();

        void
        setEndofLineChar(char eol)
        {
            m_eol = eol;
        }

        char
        getEndofLineChar()
        {
            return m_eol;
        }

        void flush();

        SerialImpl *getImpl()
        {
            return &m_impl;
        }

    private:
        SerialImpl m_impl;
        EmbyLibs::String m_dev;
        char m_eol;
        bool m_isOpen;
    };
}
#endif /* Serial_H */
