/**
 * @addtogroup EmbySystem
 * @{
 * @file K10_FreeRtos/EmbySystem/System.cc
 * @author Massimiliano Pagani
 * @version 1.0
 * @date 17/10/2014
 *
 */

#include <cstdlib>
#include <EmbyTime/Millis.hh>
#include <EmbyDebug/assert.hh>
#include <limits>

#include "mbed.h"
#include "rtx_core_cm.h"
#include "us_ticker_api.h"

namespace EmbySystem
{
    bool isInInterrupt()
    {
        return IsException();
    }

    bool areInterruptsEnabled()
    {
        return core_util_are_interrupts_enabled();
    }

    void signalInterrupt()
    {
    }

    void startScheduler(void)
    {
    }

    void stopScheduler(void)
    {
    }

    void delayMs(EmbyTime::Millis ms)
    {
//        wait_us(ms*1000);
         ThisThread::sleep_for(ms);
    }

    EmbyTime::Millis upTimeMs()
    {
        //        return HAL_GetTick();
        return Kernel::get_ms_count();
    }

    void reboot(uint32_t delayMS)
    {
        delayMs(delayMS);
        NVIC_SystemReset();
    }
} // namespace EmbySystem

///@}
