/**
 * @addtogroup EmbyClass
 * @{
 * @file EmbyLog/LogLevel.hh
 * @author Marco Garzola
 * @version 1.0
 * @date 16/10/2014
 *
 */

#ifndef EMBYLOG_LOG_CLASS_UTILS_HH
#define    EMBYLOG_LOG_CLASS_UTILS_HH

#include <EmbyLibs/String.hh>
#include <EmbyLog/LogClass.hh>

namespace EmbyLog
{
    EmbyLibs::String LogClass_toString(LogClass logClass);

    uint32_t LogCLass_fromNameToNumberId(char *p);
}

#endif

