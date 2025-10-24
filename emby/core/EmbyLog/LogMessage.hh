/**
 * @addtogroup EmbyLog
 *
 * @file EmbyLog/LogProcessor.hh
 * @author Garzola Marco
 * @version 1.0
 * @date 16/11/2014
 *
 * @notes
 * @history
 *
 */

#if !defined( EMBYLOG_LOGMESSAGE_HH )
#define EMBYLOG_LOGMESSAGE_HH

#include <EmbyLog/LogLevel.hh>
#include <EmbyTime/TimeStamp.hh>
#include <EmbyLog/LogClass.hh>
#include <EmbyLibs/String.hh>

namespace EmbyLog
{
    class LogMessage
    {
        public:

            LogMessage( EmbyLibs::String const & msg,
                        EmbyTime::TimeStamp timestamp,
                        LogLevel level,
                        LogClass logClass = LOG_CLASS_NONE);

            /**
             * Constructor with move semantic for string message. If the string
             * you have built is just for LogMessage then you may move it into
             * the LogMessage.
             *
             * @param msg the text to move into the LogMessage.
             * @param timestamp the timestamp of the message.
             * @param level the logging level.
             * @param logClass the class of the log message.
             */
            LogMessage( EmbyLibs::String&& msg,
        				EmbyTime::TimeStamp timestamp,
        				LogLevel level,
        				LogClass logClass = LOG_CLASS_NONE );

            LogMessage( LogMessage const& copy );

            LogMessage( LogMessage&& copy );

            LogMessage();

            LogMessage& operator=( LogMessage const& copy );

            ~LogMessage();

            LogLevel
            getLogLevel() const;

            EmbyTime::TimeStamp
            getTimestamp() const;

            EmbyLibs::String const&
            getText() const;

            LogClass
            getLogClass() const;

            void swap( LogMessage& other ) noexcept;

        private:
            LogLevel                m_level;
            EmbyLibs::String m_msg;
            EmbyTime::TimeStamp     m_timestamp;
            LogClass                m_class;
    };

    // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // :::::::::::::::::::::::::::::::::::::::::::::::::::: inline methods :::
    // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    inline LogLevel
    LogMessage::getLogLevel() const
    {
        return m_level;
    }

    inline EmbyTime::TimeStamp
    LogMessage::getTimestamp() const
    {
        return m_timestamp;
    }

    inline EmbyLibs::String const&
    LogMessage::getText() const
    {
        return m_msg;
    }

    inline LogClass
    LogMessage::getLogClass() const
    {
        return m_class;
    }


}   // end of namespace EmbyLog

#endif
