/**
 * @addtogroup EmbyThreading
 * @{
 * @file EmbyThreading/Semaphore_Impl.hh
 * @author apagani
 * @version 1.0
 * @date Jan 28, 2015
 */

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
