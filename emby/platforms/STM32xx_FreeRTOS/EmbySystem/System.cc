#include <cstdlib>
#include <EmbySystem/System.hh>
#include <EmbyDebug/assert.hh>


extern "C"
{
    #include "FreeRTOS.h"
    #include "task.h"
    #include "stm32xx_lib.h"
}


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


    uint32_t millisToTicks( EmbySystem::Millis ms )
    {
        return (ms * configTICK_RATE_HZ)/1000;
    }

    bool
    startScheduler(void)
    {
        vTaskStartScheduler();
        return true;
    }



    bool
    stopScheduler(void)
    {
        vTaskEndScheduler();
        return true;
    }

    void
    delayMs(EmbyTime::Millis ms)
    {

        if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
        {
            volatile unsigned long tick = millisToTicks(ms);
            vTaskDelay(tick);
        }
        else
        {
            if (ms < portTICK_PERIOD_MS)
            {
                //	ms = portTICK_PERIOD_MS;
            }

            HAL_Delay(ms);
        }

    }

    SystemUniqueId
    getSystemUniqueId()
    {
        SystemUniqueId id;
        //  std::fill( std::begin( id.m_id ), std::end( id.m_id ), 0 );
        return id;
    }

    void reboot(uint32_t delayMS)
    {
        EmbySystem::delayMs(delayMS);
        HAL_NVIC_SystemReset();
    }

    void EnterCriticalSection()
    {
        portENTER_CRITICAL();
    }

     void ExitCriticalSection()
    {
       portEXIT_CRITICAL();
    }

    bool
    isInInterrupt(void)
    {
        return (__get_IPSR() != 0U);
    }
}

///@}


