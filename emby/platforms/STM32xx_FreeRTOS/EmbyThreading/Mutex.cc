#include <EmbyThreading/Mutex.hh>
#include "FreeRTOS.h"
#include "semphr.h"
#include  "Mutex_Impl.hh"

namespace EmbyThreading
{
    Mutex:: Mutex(Type type  )
    {
		/*For now we support only recursive mutex, assert otherwise*/
		EmbyDebug_ASSERT(type == EmbyThreading::Mutex::Type::Recursive );
		m_impl.m_handle = xSemaphoreCreateRecursiveMutex();
		EmbyDebug_ASSERT(m_impl.m_handle != nullptr);
	}

	void
	Mutex::release()
	{
		signed portBASE_TYPE retval;
		retval = xSemaphoreGiveRecursive(m_impl.m_handle);
		EmbyDebug_ASSERT(retval = pdTRUE);
	}

	void
	Mutex::acquire()
	{
	    signed portBASE_TYPE retval =
	        xSemaphoreTakeRecursive(m_impl.m_handle, portMAX_DELAY);
	    EmbyDebug_ASSERT(retval = pdTRUE);
	}

    bool
    Mutex::tryAcquire(EmbyTime::Millis timeout)
    {
        return xSemaphoreTakeRecursive(
                       m_impl.m_handle,
                       static_cast<TickType_t>(timeout / portTICK_PERIOD_MS)) == pdTRUE;
    }
}
