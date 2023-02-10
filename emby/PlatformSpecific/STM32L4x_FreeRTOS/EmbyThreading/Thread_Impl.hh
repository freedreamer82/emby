#if !defined(EMBYTHREAD_FREERTOS_IMPL_H)
#define EMBYTHREAD_FREERTOS_IMPL_H

#include "FreeRTOS.h"
#include "task.h"

namespace EmbyThreading
{
	static int constexpr  PRIORITY_LOWEST = tskIDLE_PRIORITY;
    static int constexpr PRIORITY_HIGHEST =  configMAX_PRIORITIES -1;
	struct ThreadImpl
	{
		TaskHandle_t  m_handle;
	};
}

#endif
