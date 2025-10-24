
#include <EmbyLog/LogMessage.hh>

namespace EmbyLog
{
    LogMessage::LogMessage( EmbyLibs::String const & msg,
							EmbyTime::TimeStamp timestamp,
							LogLevel level,
							LogClass logClass ) :
				m_level(level),
				m_msg(msg),
				m_timestamp(timestamp),
				m_class( logClass )
    {}

    LogMessage::LogMessage( EmbyLibs::String&& msg,
							EmbyTime::TimeStamp timestamp,
							LogLevel level,
							LogClass logClass ) :
				m_level(level),
				m_msg(std::move(msg)),
				m_timestamp(timestamp),
				m_class( logClass )
	{}

    LogMessage::LogMessage( LogMessage&& copy ) :
            m_level( copy.m_level ),
            m_msg( std::move( copy.m_msg )),
            m_timestamp( copy.m_timestamp ),
            m_class( copy.m_class )
    {
    }

    LogMessage::LogMessage( LogMessage const& copy ) :
            m_level( copy.m_level ),
            m_msg( copy.m_msg ),
            m_timestamp( copy.m_timestamp ),
            m_class( copy.m_class )
    {
    }

    LogMessage::LogMessage() : LogMessage( "", 0, LogLevel::None, 0 )
    {
    }

    LogMessage& LogMessage::operator=( LogMessage const& copy )
    {
        LogMessage tmp( copy );
        swap( tmp );
        return *this;
    }

    LogMessage::~LogMessage()
    {
        m_level = LogLevel::None;
        m_timestamp = 0;
    }

    void LogMessage::swap( LogMessage& other ) noexcept
    {
        std::swap( m_level, other.m_level );
        m_msg.swap( other.m_msg );
        std::swap( m_timestamp, other.m_timestamp );
        std::swap( m_class, other.m_class );
    }

}  // end of namespace EmbyLog
///@}


