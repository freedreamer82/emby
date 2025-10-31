#if !defined(EMBY_THREADING_SEMAPHORE_HH_)
#error "Do not include this file directly, include <EmbyThreading/Semaphore.hh> instead"
#endif

#include "mbed.h"

namespace EmbyThreading
{
    struct SemaphoreImpl
    {
        Semaphore sem;
        bool      isWaitingSignal;
    };

} // namespace EmbyThreading
