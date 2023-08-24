#if !defined( EMBY_THREADING_SEMAPHORE_HH_ )
#  error "Do not include this file directly, include <EmbyThreading/Semaphore.hh> instead"
#endif

#include <semaphore.h>

namespace EmbyThreading
{
    typedef sem_t SemaphoreImpl;
}
