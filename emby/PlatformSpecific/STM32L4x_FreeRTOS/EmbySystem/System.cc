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
#include <EmbyThreading/Semaphore.hh>
#include <unistd.h>
#include <limits>
#include <FreeRTOS.h>
#include "task.h"
//#include "cmsis_device.h"

extern "C"
{
#include "stm32l4xx.h"
}


#define STM32_UUID ((uint32_t *)UID_BASE) //stm32F0x micro


namespace EmbySystem
{
    //#define SCB_ICSR_VECTACTIVE_Msk            (0x1FFUL /*<< SCB_ICSR_VECTACTIVE_Pos*/)       /*!< SCB ICSR: VECTACTIVE Mask */

    namespace
    {
        EmbyThreading::Semaphore g_event;
    }


    bool
    waitInterrupt(EmbyTime::Millis timeout)
    {
        g_event.wait(timeout);
        return false;
    }

    void
    signalInterrupt()
    {
        g_event.signal();
    }

    void
    startScheduler(void)
    {
        vTaskStartScheduler();
    }

    void
    stopScheduler(void)
    {
        //disable systick
        if (SysTick->CTRL & SysTick_CTRL_ENABLE_Msk)
        {
            SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
        }
        vTaskEndScheduler();
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

    EmbyTime::Millis
    upTimeMs()
    {
        return ((unsigned long long) xTaskGetTickCount() * portTICK_RATE_MS);
    }

    SystemUniqueId
    getSystemUniqueId()
    {
        SystemUniqueId id; //12 byte in Stm32
        id.m_id[0] = STM32_UUID[0];
        id.m_id[1] = STM32_UUID[1];
        id.m_id[2] = STM32_UUID[2];
        id.m_id[3] = 0;

        return id;
    }

    bool
    isInInterrupt(void)
    {
        return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
    }


    void go2sleep()
    {
//        HAL_PWR_EnterSTANDBYMode();
//        EXTI->PR1 = 0xFFFFFFFF;
//        EXTI->PR1 = 0xFFFFFFFF;
        HAL_SuspendTick();
        HAL_PWR_EnableSleepOnExit();

        HAL_PWREx_EnterSTOP0Mode (PWR_STOPENTRY_WFI);
    }

    void reboot(uint32_t delayMS)
    {
        EmbySystem::delayMs(delayMS);
        HAL_NVIC_SystemReset();
    }

    bool isCoreSleeping()
    {
        return false;
    }

      uint32_t
    millisToTicks( EmbyTime::Millis ms )
    {
        return (ms * configTICK_RATE_HZ)/1000;
    }


      EmbyTime::Millis
    ticksToMillis( uint32_t ticks )
    {
        return (ticks*1000)/configTICK_RATE_HZ;
    }
}

///@}


