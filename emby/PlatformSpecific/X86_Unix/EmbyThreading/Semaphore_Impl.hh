#if !defined(EMBY_THREADING_SEMAPHORE_HH_)
#error "Do not include this file directly, include <EmbyThreading/Semaphore.hh> instead"
#endif

#ifdef __APPLE__
#include "pthread.h"

typedef struct sem_t
{
    pthread_mutex_t count_lock;
    pthread_cond_t count_bump;
    unsigned count;
} sem_t;
#else

#include <semaphore.h>

#endif

namespace EmbyThreading
{
    struct SemaphoreImpl
    {
        sem_t sem;
    };
}
