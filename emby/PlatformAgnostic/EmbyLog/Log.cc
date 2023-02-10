#include <EmbyLog/Log.hh>

namespace EmbyLog
{
    using EmbyLibs::sprintf;


    void log(LogLevel level, LogClass logClass, char const *text)
    {
        LogBus::get().log(level, logClass, text);
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

    void logPrintf(LogLevel level, LogClass logClass, char const *fmt, ...)
    {
        //  try
        {
            va_list args;
            va_start(args, fmt);
            logVPrintf(level, logClass, fmt, args);
            va_end(args);
        }
        //	catch (std::bad_alloc const&)
        {
            // do nothing...log lost!
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
