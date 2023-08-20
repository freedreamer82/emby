
#include <EmbyThreading/Semaphore.hh>
#include <EmbySystem/System.hh>
#include <EmbyDebug/assert.hh>
#include "FreeRTOS.h"
#include "semphr.h"

namespace EmbyThreading
{

    Semaphore::Semaphore() :
               m_impl{ xSemaphoreCreateBinary(), false }
    {
    	EmbyDebug_ASSERT_CHECK_NULL_PTR(m_impl.semaphore);
    }

    Semaphore::~Semaphore()
    {
        vSemaphoreDelete( m_impl.semaphore );
    }


    bool Semaphore::signal()
    {
    	/* We have not woken a task at the start of the ISR. */
		BaseType_t xHigherPriorityTaskWoken =  pdTRUE;

		if ( EmbySystem::isInInterrupt() )
		{
			xSemaphoreGiveFromISR( m_impl.semaphore, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	        return xHigherPriorityTaskWoken;
		}
		else
		{
			bool someoneHasBeenSignaled = xSemaphoreGive( m_impl.semaphore );
	        return someoneHasBeenSignaled;
		}
    }

    bool Semaphore::wait( EmbyTime::Millis timeout )
    {
        volatile auto ticks = EmbySystem::millisToTicks( timeout );
        m_impl.isWaitingSignal = true;
        bool isTaken = (xSemaphoreTake( m_impl.semaphore, ticks ) == pdTRUE);
        m_impl.isWaitingSignal = false;
        return isTaken;
    }

    bool Semaphore::isSomeoneWaiting() const
    {
        return m_impl.isWaitingSignal;
    }

} /* namespace EmbyThreading */
