#if !defined(EMBYMUTEX_MBED_IMPL_H)
#define EMBYMUTEX_MBED_IMPL_H

#include "mbed.h"

namespace EmbyThreading
{
    struct MutexImpl
    {
        rtos::Mutex mutex;
    };
} // namespace EmbyThreading

#endif // EMBYMUTEX_MBED_IMPL_H
