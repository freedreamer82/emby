#if !defined( EMBY_THREADING_SEMAPHORE_HH_ )
#  error "Do not include this file directly, include <EmbyThreading/Semaphore.hh> instead"
#endif

#include <FreeRTOS.h>
#include <semphr.h>

namespace EmbyThreading
{
    struct SemaphoreImpl
    {
        SemaphoreHandle_t semaphore;
        bool 			  isWaitingSignal;
    };

}
