/**
 * @addtogroup EmbyLog
 *
 *
 * @file EmbyLog/LogProcessorQueue.hh
 * @author Marco Garzola
 * @version 1.0
 * @date 11/1/2014
 *
 * @notes
 * @history
 *
 */

#if !defined( EMBYLOG_LOGPROCESSOR_QUEUE_HH )
#define EMBYLOG_LOGPROCESSOR_QUEUE_HH

#include <EmbyLog/LogMessage.hh>
#include <EmbyLog/LogProcessor.hh>
#include <EmbyLog/LogLevel.hh>
#include <EmbyLibs/CircularBuffer.hh>
#include <vector>

namespace EmbyLog
{
    #define LOG_QUEUE_SIZE                      20 ///@todo: move it in defines.

    class LogProcessorQueue : public LogProcessor
    {
        public:

            LogProcessorQueue(EmbyLog::LogMask levelMask);

            virtual ~LogProcessorQueue();

            bool
            deQueue( LogMessage * logMessage);

            void
            clearQueue()
            {
                m_queue.clear();
                m_lostMessagesCounter = 0;
            }

        protected:

        private:
            std::vector<LogProcessor*>                                  m_processors;
            EmbyLibs::CircularBuffer<LogMessage , LOG_QUEUE_SIZE>     m_queue;
            uint16_t                                                    m_lostMessagesCounter;

            virtual void
            onLog( LogLevel level,
            	   LogClass logClass,
            	   EmbyLibs::String const& message );

            virtual void
            waitLogEnd();
    };
}
#endif
