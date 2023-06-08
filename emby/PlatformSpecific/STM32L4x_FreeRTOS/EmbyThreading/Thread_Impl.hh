#if !defined(EMBYTHREAD_FREERTOS_IMPL_H)
#define EMBYTHREAD_FREERTOS_IMPL_H

#include "FreeRTOS.h"
#include "task.h"

namespace EmbyThreading
{

	struct ThreadImpl
	{
		TaskHandle_t  m_handle;
	};
}

#endif
