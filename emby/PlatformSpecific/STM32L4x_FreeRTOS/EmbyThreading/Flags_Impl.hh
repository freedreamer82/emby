#if !defined(EMBYFLAGS_FREERTOS_IMPL_H)
#define EMBYFLAGS_FREERTOS_IMPL_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include <EmbyThreading/Semaphore.hh>

namespace EmbyThreading
{
	struct FlagsImpl
	{
		//EventGroupHandle_t  eventFlags;
		EmbyThreading::Semaphore	 semaphore;
	};
}

#endif
