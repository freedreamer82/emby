

#if !defined( EMBYLOG_LOGPROCESSOR_HH )
#define EMBYLOG_LOGPROCESSOR_HH

#include <EmbyLog/LogMessage.hh>
#include <EmbyLog/LogClass.hh>
#include <EmbyLibs/String.hh>

namespace EmbyLog
{

/** EmbyLog/LogProcessor
 *
 * The log processor is a generic log messages consumer. It is expected to take
 * a message and deliver it in the proper place (log file, log server,
 * /dev/null...).
 *
 */

class LogProcessor
{
    public:

    	LogProcessor(LogMask levelMask) noexcept;

        virtual ~LogProcessor() noexcept;

        void
        log( EmbyLog::LogLevel level,
        	 LogClass logClass,
        	 EmbyLibs::String const& message );

        /**
         * return the Log Level Mask handled by the processor
         *
         * @return the level mask.
         */
        LogMask
        getLevelMask() const noexcept;

        /**
         * set the Log Level Mask handled by the processor
         *
         * @param mask the level mask to set.
         */
        void
        setLevelMask(EmbyLog::LogMask mask) noexcept;

        /**
         * Synchronous method to wait for all messages to be written to the
         * the target logging support.
         */
        void flush();
    private:
        LogMask m_levelMask;

        virtual void onLog( EmbyLog::LogLevel level,
                            LogClass logClass,
                            EmbyLibs::String const& message ) = 0;
        virtual void waitLogEnd() = 0;
};

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::: inline methods :::
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline
LogProcessor::LogProcessor( LogMask levelMask) noexcept
			  : m_levelMask(levelMask)
{}

inline void
LogProcessor::log( EmbyLog::LogLevel level,
                   LogClass logClass,
                   EmbyLibs::String const& message )
{
	onLog( level, logClass, message );
}

inline EmbyLog::LogMask
LogProcessor::getLevelMask() const noexcept
{
	return m_levelMask;
}

inline void
LogProcessor::setLevelMask(EmbyLog::LogMask mask) noexcept
{
	 m_levelMask = mask;
}

inline void
LogProcessor::flush()
{
    waitLogEnd();
}

}   // end of namespace EmbyLog

#endif
