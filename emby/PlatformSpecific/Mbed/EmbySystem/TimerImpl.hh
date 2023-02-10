#ifndef EMBY_SYSTEM_TIMER_IMPL_H
#define EMBY_SYSTEM_TIMER_IMPL_H

#include <mbed.h>

namespace EmbySystem
{
    class Timer;

    struct TimerImpl
    {
        bool     running;
        bool     periodic;
        uint32_t durationMs;
        Thread*  thread;
        Timer*   timer;
        Ticker   ticker;
        char*    threadName;

        void threadCallback();
        void tickerCallback();
    };

} // namespace EmbySystem

#endif // EMBY_SYSTEM_TIMER_IMPL_H
