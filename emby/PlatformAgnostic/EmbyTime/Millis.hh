/**
 * @addtogroup EmbyExec
 * @{
 * @file EmbyExec/Millis.hh
 * @author Massimiliano Pagani
 * @version 1.0
 * @date 16/10/2014
 *
 */

#ifndef EMBYEXEC_MILLIS_HH
#define	EMBYEXEC_MILLIS_HH

#include <cstdint>

namespace EmbyTime
{

    /**
     * A type to hold a time value. Currently implemented as milliseconds. It
     * could be replaced by a compatible, but safer implementation. For now,
     * you are on your own.
     *
     * Being implemented as a 64bit int you don't have to worry (at least, not
     * for a long long time) about wrap around.
     */
    typedef std::uint32_t Millis;

    constexpr inline Millis operator ""_s( unsigned long long int seconds )
    {
        uint32_t constexpr MILLIS_IN_A_SECOND = 1000;
        return seconds*MILLIS_IN_A_SECOND;
    }

    constexpr inline Millis operator ""_min( unsigned long long int minutes )
    {
        uint32_t constexpr MILLIS_IN_A_MINUTE = 60*1000;
        return minutes*MILLIS_IN_A_MINUTE;
    }

    constexpr inline Millis operator ""_ms(  unsigned long long int millis )
    {
        return millis;
    }
}   // namespace EmbyExec

#endif	/* EMBYEXEC_MILLIS_HH */

