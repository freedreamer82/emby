/**
 * @addtogroup EmbyLog
 *
 * @author Massimiliano Pagani
 * @version 1.0
 * @date 18/05/2007
 * @file EmbyLog/LogBus.hh
 *
 * @notes
 * @history
 *
 */

#if !defined( EMBYLOG_LOGBUS_HH )
#define EMBYLOG_LOGBUS_HH

#include <EmbyLibs/Singleton.hh>
#include <EmbyLog/LogProcessor.hh>
#include <vector>
#include <EmbyLog/LogLevel.hh>
#include <EmbyLog/LogClass.hh>
#include <EmbyThreading/Mutex.hh>
#include <EmbyLibs/String.hh>

namespace EmbyLog
{


    /**
     *
     * This is the core logging bus. This singleton is the connector between
     * the logrequest and log processors.
     */

    class LogBus : public EmbyLibs::Singleton<LogBus>
    {
        public:
            LogBus();
            ~LogBus();

            void log( EmbyLog::LogLevel level,
                      EmbyLog::LogClass logClass,
                      EmbyLibs::String const& message );

            void log( EmbyLog::LogLevel level,
                      EmbyLog::LogClass logClass,
                      EmbyLibs::String&& message );

            void registerProcessor( LogProcessor* processor );
            void unregisterProcessor( LogProcessor* processor );
            bool isRegistered( LogProcessor* processor );

            /**
             * Synchronous method to wait for all messages to be written to the
             * the target logging support.
             */
            void flush();
        private:
            LogBus( const LogBus& copy );
            LogBus& operator=( const LogBus& copy );

            std::vector<LogProcessor*> m_processors;
            EmbyThreading::Mutex m_processorMutex;
    };

}   // end of namespace EmbyLog
#endif
