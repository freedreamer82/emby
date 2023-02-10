/**
 * @addtogroup EmbyLog
 * @{
 * @file EmbyLog/LogLevel.hh
 * @author Marco Garzola
 * @version 1.0
 * @date 16/10/2014
 *
 */

#ifndef EMBYEXEC_LOG_LEVEL_HH
#define EMBYEXEC_LOG_LEVEL_HH

#include <cstdint>

namespace EmbyLog
{

    typedef uint16_t LogMask;
//    typedef uint16_t LogLevel;

    enum class LogLevel
    {
        None     = 0,
        Fatal    = 1,
        Error    = 2,
        Warning  = 3,
        Info     = 4,
        Debug    = 5,
        Trace    = 6,
        Reserved = 7
    };

    inline constexpr const char *levelToString(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::Debug:
                return "Debug";
            case LogLevel::Info:
                return "Info";
            case LogLevel::Warning:
                return "Warning";
            case LogLevel::Trace:
                return "Trace";
            case LogLevel::Reserved:
                return "Reserved";
            case LogLevel::Fatal:
                return "Fatal";
            case LogLevel::Error:
                return "Error";
            default:
                return "None";
        }
    }

    inline constexpr LogMask
    logMaskOf( LogLevel
    level )
{
    return (level == LogLevel::Reserved || level == LogLevel::None) ?
    0 :
    (1u << static_cast
    <uint16_t>( level )
    );
}

inline constexpr LogMask
logMaskFrom( LogLevel
level )
{
return level == LogLevel::Reserved ?
0 :
((1u << (static_cast
<uint16_t>( level )
+1u))-1u) & ~1u;
}


/**
* The level describes the severity of the message to log.
* @note the odd naming is because LOG_* are preprocessor symbol
*       defined by system includes.
*  The level could also be used as flags. LOG_FATAL | LOG_NONE
*/
namespace LevelMask
{
    static LogMask constexpr
    None = logMaskOf(LogLevel::None);
    static LogMask constexpr
    Fatal = logMaskOf(LogLevel::Fatal);
    static LogMask constexpr
    Error = logMaskOf(LogLevel::Error);
    static LogMask constexpr
    Warning = logMaskOf(LogLevel::Warning);
    static LogMask constexpr
    Info = logMaskOf(LogLevel::Info);
    static LogMask constexpr
    Debug = logMaskOf(LogLevel::Debug);
    static LogMask constexpr
    Trace = logMaskOf(LogLevel::Trace);
    static LogMask constexpr
    Reserved = logMaskOf(LogLevel::Reserved);
    static LogMask constexpr
    All = Fatal |
          Error |
          Warning |
          Info |
          Debug |
          Trace;

    static LogMask constexpr
    From_Fatal = logMaskFrom(LogLevel::Fatal);
    static LogMask constexpr
    From_Error = logMaskFrom(LogLevel::Error);
    static LogMask constexpr
    From_Warning = logMaskFrom(LogLevel::Warning);
    static LogMask constexpr
    From_Info = logMaskFrom(LogLevel::Info);
    static LogMask constexpr
    From_Debug = logMaskFrom(LogLevel::Debug);
    static LogMask constexpr
    From_Trace = logMaskFrom(LogLevel::Trace);
};
}

#endif

