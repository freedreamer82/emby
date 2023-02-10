
#include <EmbyLog/LogProcessorQueue.hh>
#include <EmbyLog/LogMessage.hh>
#include <EmbyTime/TimeStamp.hh>
#include <EmbyTime/Clock.hh>
#include <EmbyTime/DateTime.hh>
#include <EmbyDebug/assert.hh>
#include <EmbyLibs/StringUtils.hh>
#include <EmbySystem/System.hh>

namespace EmbyLog
{
    LogProcessorQueue::LogProcessorQueue(EmbyLog::LogMask levelMask)
            : LogProcessor(levelMask), m_lostMessagesCounter(0)
    {
        clearQueue();
    }

    LogProcessorQueue::~LogProcessorQueue()
    {
    }

    void
    LogProcessorQueue::onLog(LogLevel level, LogClass logClass, EmbyLibs::String const &message)
    {
        if (message.data())
        {
            EmbyLibs::String msgCopy = message; // make a copy and move it if possible.
            if (msgCopy.data())
            {
                EmbyLog::LogMessage msg(
                        std::move(msgCopy), EmbySystem::upTimeMs(), level, logClass);

                if (!m_queue.isFull())
                {
                    m_queue.push(std::move(msg));
                }
                else
                {
                    m_lostMessagesCounter++;
                }
            }
            else
            {
                m_lostMessagesCounter++;
            }
        }
        else
        {
            m_lostMessagesCounter++;
        }
    }

    void LogProcessorQueue::waitLogEnd()
    {
        while (!m_queue.isEmpty())
        {
            EmbySystem::delayMs(1);
        }
    }

    bool LogProcessorQueue::deQueue(LogMessage *logMessage)
    {
        EmbyDebug_ASSERT_CHECK_NULL_PTR(logMessage);

        bool retval = false;

        if (!m_queue.isEmpty())
        {
            *logMessage = m_queue.popMove(); // cannot die . it's a move!

            if (m_lostMessagesCounter != 0)
            {
                /* DAMN IT we have to push a warning , we lost some messages...
                 * We should use mutex 'cause another thread could call the log function...BUT to
                 * simplify the case we use critical section , theoretically this case shouldn't
                 * happen so often.
                 * */
                auto now = EmbySystem::upTimeMs();
                ///@TODO: is this necessary since we have circular buffer with critical section???
                EmbySystem::EnterCriticalSection();
                if (!m_queue.isFull())
                {
                    LogMessage msg("Lost Messages", now, LogLevel::Warning);
                    EmbyDebug_ASSERT(m_lostMessagesCounter != 0);
                    m_queue.push(msg);
                    m_lostMessagesCounter = 0;
                }
                EmbySystem::ExitCriticalSection();
            }
            retval = true;
        }
        return retval;
    }

} // namespace EmbyLog
/// @}
