

#include <EmbySystem/Timer.hh>
#include <EmbySystem/System.hh>
#include <EmbyDebug/assert.hh>
#include "Timer_Impl.hh"

namespace EmbySystem {
    static void TimerCallbackFunctionAdapter(TimerHandle_t xTimer) {
        Timer *timer = static_cast<Timer *>(pvTimerGetTimerID(xTimer));
        if (timer->isActive()) {
            timer->callback(*timer);
        }
    }


    uint16_t Timer::getPeriod() {
        auto ticks = xTimerGetPeriod(m_impl.m_handle);
        return ticksToMillis(ticks);
    }


    Timer::Timer(TimerSize ms, bool isPeriodic,Type timerT)
    {
        EmbyDebug_ASSERT(timerT == Type::Software);
        auto ticks = EmbySystem::millisToTicks(ms);
        if (ticks == 0)
            ticks = portMAX_DELAY;

        m_impl.m_handle = xTimerCreate("timer",
                                       ticks,
                                       isPeriodic ? pdTRUE : pdFALSE,
                                       this,
                                       TimerCallbackFunctionAdapter);

        EmbyDebug_ASSERT_CHECK_NULL_PTR(m_impl.m_handle);
        m_impl.isRunning = false;
    }


    Timer::~Timer() {
        xTimerDelete(m_impl.m_handle, portMAX_DELAY);
    }

    bool Timer::isActive() {
        bool isActive = xTimerIsTimerActive(m_impl.m_handle) == pdFALSE ? false : true;
        return isActive && m_impl.isRunning;
    }

    bool Timer::start(TimerSize CmdTimeout) {
        BaseType_t xHigherPriorityTaskWoken = pdTRUE;

        m_impl.isRunning = true;
        if (EmbySystem::isInInterrupt()) {
            auto ret = xTimerStartFromISR(m_impl.m_handle, &xHigherPriorityTaskWoken) == pdFALSE ? false : true;
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            return ret;
        } else {
            return xTimerStart(m_impl.m_handle, CmdTimeout) == pdFALSE ? false : true;

        }
    }

    bool Timer::stop(TimerSize CmdTimeout) {
        BaseType_t xHigherPriorityTaskWoken = pdTRUE;

        m_impl.isRunning = false;
        if (EmbySystem::isInInterrupt()) {
            auto ret = xTimerStopFromISR(m_impl.m_handle, &xHigherPriorityTaskWoken) == pdFALSE ? false : true;
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            return ret;
        } else {
            return xTimerStop(m_impl.m_handle, CmdTimeout) == pdFALSE ? false : true;

        }
    }

    bool Timer::reset(TimerSize CmdTimeout) {
        BaseType_t xHigherPriorityTaskWoken = pdTRUE;
        if (EmbySystem::isInInterrupt()) {
            auto ret = xTimerResetFromISR(m_impl.m_handle, &xHigherPriorityTaskWoken) == pdFALSE ? false : true;
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            return ret;
        } else {
            return xTimerReset(m_impl.m_handle, CmdTimeout) == pdFALSE ? false : true;

        }
    }


    bool Timer::setPeriod(TimerSize NewPeriod,
                          TimerSize CmdTimeout) {


        BaseType_t xHigherPriorityTaskWoken = pdTRUE;
        if (EmbySystem::isInInterrupt()) {
            auto ret = (xTimerChangePeriodFromISR(m_impl.m_handle, NewPeriod, &xHigherPriorityTaskWoken) == pdFALSE)
                       ? false
                       : true;
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            return ret;
        } else {
            return (xTimerChangePeriod(m_impl.m_handle, NewPeriod, CmdTimeout) == pdFALSE)
                   ? false : true;

        }

    }
}