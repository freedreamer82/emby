/**
 * @addtogroup EmbyClass
 * @{
 * @file EmbyLog/LogLevel.hh
 * @author Marco Garzola
 * @version 1.0
 * @date 16/10/2014
 *
 */

#ifndef EMBYLOG_LOG_CLASS_HH
#define	EMBYLOG_LOG_CLASS_HH

#include <EmbyLibs/fourcc.hh>

/**
 * Macro to define the log class for a module. Just instantiate the class at
 * the beginning of the .cc file to define the class of logging for that
 * compilation unit.
 * 
 * This may cause some problems with inline functions, but consider that you
 * shouldn't use logging (which is potentially time consuming) within an inline
 * function (which is expected to be short and quick).
 */
#define EmbyLog_MODULE_LOG_CLASS( Name )                                \
    namespace EmbyLog                                                   \
    {                                                                   \
        static const LogClass g_logClass = EmbyLibs::fourcc(Name);    \
    }

namespace EmbyLog
{
    typedef uint32_t LogClass;
    
    LogClass constexpr LOG_CLASS_NONE = EmbyLibs::fourcc( "----" );
    
#define EmbyLog_GET_MODULE_LOG_CLASS()                      \
    (EmbyLog::g_logClass)

}
#endif

