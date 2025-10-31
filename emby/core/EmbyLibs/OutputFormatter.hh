
#ifndef EMBY_PLATFORMAGNOSTIC_EMBYCONSOLE_OUTPUTFORMATTER_H_
#define EMBY_PLATFORMAGNOSTIC_EMBYCONSOLE_OUTPUTFORMATTER_H_

#include <EmbyLibs/IOutput.hh>
#include <cstdarg>

namespace EmbyLibs
{
    void printf( IOutput& out, char const* fmt, ... )
         __attribute__ (( format (printf, 2, 3 ) ));

    void vprintf( IOutput& out, char const* fmt, va_list args );
}   // end of namespace EmbyLibs

#endif /* EMBY_PLATFORMAGNOSTIC_EMBYCONSOLE_OUTPUTFORMATTER_H_ */

/**
 * @}
 */
