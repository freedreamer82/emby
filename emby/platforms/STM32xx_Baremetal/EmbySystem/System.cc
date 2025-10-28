#include <cstdlib>
#include <EmbySystem/System.hh>
#include <EmbyDebug/assert.hh>
#include "stm32xx_lib.h"

static bool g_isRunning = false;

namespace EmbySystem
{
    using EmbyTime::Millis;

    void abort()
    {
        ::abort();
    }


    Millis upTimeMs()
    {
        return HAL_GetTick();
    }


    bool
    startScheduler(void)
    {
        return false;
    }

    bool
    stopScheduler(void)
    {
        return false;
    }

    void
    delayMs(Millis ms)
    {
        HAL_Delay(ms);
    }

    SystemUniqueId
    getSystemUniqueId()
    {
        SystemUniqueId id;
        //  std::fill( std::begin( id.m_id ), std::end( id.m_id ), 0 );
        return id;
    }

    void reboot(uint32_t ms)
    {
        if (ms != 0)
        {
            delayMs(ms);
        }
        HAL_NVIC_SystemReset();
    }


    bool
    isInInterrupt(void)
    {
        return false;
    }

    void EnterCriticalSection()
    {
        // do things
    }

    void ExitCriticalSection()
    {
        // do things
    }

}

///@}


