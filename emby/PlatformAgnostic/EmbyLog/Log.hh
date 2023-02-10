/** 
 * @addtogroup EmbyLog
 * @ingroup EmbyLog
 *
 * @author Massimiliano Pagani
 * @version 1.0
 * @date 18/05/2007
 *
 * @notes
 * @history
 *
 */

#if !defined( EMBY_LOG_HH )
#define EMBY_LOG_HH

#include <EmbyLog/LogBus.hh>
#include <EmbyLibs/String.hh>
#include <EmbyLog/LogLevel.hh>
#include <stdarg.h>
#include <EmbyLibs/StringUtils.hh>

/** 
 *
 * This namespace is just a shortcut for using the logging system.
 *
 */

namespace EmbyLog
{

    /**
     * Unformatted logging. Just log the given text.
     *
     * @param level the logging level.
     * @param logClass the logging class.
     * @param text the message to print on the log support.
     *
     * @{
     */
    void log( LogLevel level, LogClass logClass, char const* text );

    void log( LogLevel level,
              LogClass logClass,
              EmbyLibs::String const& text );

    void log( LogLevel level,
              LogClass logClass,
              EmbyLibs::String&& text );
    /**
     * @}
     */

    /**
     * #va_list lower level logging function. This function always logs.
     *
     * @param level the logging level.
     * @param logClass the logging class.
     * @param fmt the formatting string (as in printf).
     * @param args a va_list type for providing arguments to the formatting
     *             string.
     *
     * @note this call is asynchronous, i.e. it returns before the message has
     *       been completely written to the logging support. @see flush().
     */
    void logVPrintf( LogLevel level,
                     LogClass logClass,
                     char const* fmt,
                     va_list args );

    /**
     * Free arguments formatted logging. This function always logs.
     *
     * @param level the logging level.
     * @param logClass the logging class.
     * @param fmt the formatting string (as in printf).
     * @param ... Free argument list that will be used by the formatting string.
     * @param text the string to log.
     *
     * @note this call is asynchronous, i.e. it returns before the message has
     *       been completely written to the logging support. @see flush().
     * @{
     */
    void logPrintf( LogLevel level, LogClass logClass, char const* fmt, ... );

    void logPrintf( LogLevel level,
                    LogClass logClass,
                    EmbyLibs::String const& text );

    void logPrintf( LogLevel level,
                    LogClass logClass,
                    EmbyLibs::String&& text );
    /**
     * @}
     */
    /**
     * This function causes the log system to completely output the current
     * queues. This call is synchronous, meaning that it returns to the caller
     * only when the last character has been moved out of the logging system
     * onto the logging support.
     */
    void flush();

#if defined( EMBY_DISABLE_LOGS )
    #define log_trace( ...)
    #define log_debug( ...)
    #define log_info(...)
    #define log_warning( ...)
    #define log_error( ...)
    #define log_fatal( ...)
#else
    #define log_trace(...)   EmbyLog::logPrintf( EmbyLog::LogLevel::Trace,\
                                                 EmbyLog_GET_MODULE_LOG_CLASS(), \
                                                 __VA_ARGS__ )
    #define log_debug(...)   EmbyLog::logPrintf( EmbyLog::LogLevel::Debug, \
                                                 EmbyLog_GET_MODULE_LOG_CLASS(), \
                                                 __VA_ARGS__)
    #define log_info(...)	 EmbyLog::logPrintf( EmbyLog::LogLevel::Info, \
                         	                     EmbyLog_GET_MODULE_LOG_CLASS(), \
                         	                     __VA_ARGS__)
    #define log_warning(...) EmbyLog::logPrintf( EmbyLog::LogLevel::Warning, \
                                                 EmbyLog_GET_MODULE_LOG_CLASS(), \
                                                 __VA_ARGS__)
    #define log_error(...)   EmbyLog::logPrintf( EmbyLog::LogLevel::Error, \
                                                 EmbyLog_GET_MODULE_LOG_CLASS(), \
                                                 __VA_ARGS__)
    #define log_fatal(...)   EmbyLog::logPrintf( EmbyLog::LogLevel::Fatal, \
                                                 EmbyLog_GET_MODULE_LOG_CLASS(), \
                                                 __VA_ARGS__)
#endif
}   // end of namespace EmbyLog

#endif
