#if !defined(EMBYMUTEX_FREERTOS_IMPL_H)
#define EMBYMUTEX_FREERTOS_IMPL_H

#include "FreeRTOS.h"
#include "semphr.h"

namespace EmbyThreading
{
	struct MutexImpl
	{
		xSemaphoreHandle m_handle;
	};
}

#endif
