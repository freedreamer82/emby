#if !defined(EMBYSEM_FREERTOS_IMPL_H)
#define EMBYSEM_FREERTOS_IMPL_H


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
#endif