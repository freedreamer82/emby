#include <EmbyLog/Log.hh>
#include <EmbyLibs/CircularBuffer.hh>

namespace EmbyLog
{
    using EmbyLibs::sprintf;

#ifdef EMBY_CFG_LOG_IRQ_BUFFER
#ifndef EMBY_CFG_LOG_IRQ_BUFFER_SIZE
    #define EMBY_CFG_LOG_IRQ_BUFFER_SIZE 5
#endif
    static EmbyLibs::CircularBuffer<LogMessage, EMBY_CFG_LOG_IRQ_BUFFER_SIZE> g_irqLogBuffer;
#endif

    void log(LogLevel level, LogClass logClass, char const *text)
    {
        LogBus::get().log(level, logClass, text);
    }

    void logPrintf(LogLevel level, LogClass logClass, char const *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);

#ifdef EMBY_CFG_LOG_IRQ_BUFFER
        if (EmbySystem::isInInterrupt())
        {
            // In IRQ context: buffer the log message without formatting
            LogMessage msg(EmbyLibs::String(fmt), 0, level, logClass);
            g_irqLogBuffer.push(msg);
        }
        else
        {
            for (auto &msg: g_irqLogBuffer)
            {
                logPrintf(msg.getLogLevel(), msg.getLogClass(), msg.getText());
            }
            g_irqLogBuffer.clear();
            logVPrintf(level, logClass, fmt, args);
        }
#else
    /* EMBY_CFG_LOG_IRQ_BUFFER not defined: IRQ check disabled --
     * in IRQ context the message is discarded to avoid non-safe calls */
    if (EmbySystem::isInInterrupt())
    {
        (void)fmt; (void)level; (void)logClass;
    }
    else
    {
        logVPrintf(level, logClass, fmt, args);
    }
#endif

        va_end(args);
    }

    void logVPrintf(LogLevel level,
                    LogClass logClass,
                    char const *fmt,
                    va_list args)
    {
        EmbyLibs::String message;

        {
            EmbyLibs::sprintf(message, fmt, args);
            if (message.data())
            {
                LogBus::get().log(level, logClass, std::move(message));
            }
            else
            {
                //no memory...
            }
        }
    }

    void logPrintf(LogLevel level,
                   LogClass logClass,
                   EmbyLibs::String const &text)
    {
        LogBus::get().log(level, logClass, text);
    }

    void logPrintf(LogLevel level,
                   LogClass logClass,
                   EmbyLibs::String &&text)
    {
        EmbyLibs::String message(text);
        if (message.data())
        {
            LogBus::get().log(level, logClass, std::move(message));
        }
    }

    void flush()
    {
        LogBus::get().flush();
    }
}
