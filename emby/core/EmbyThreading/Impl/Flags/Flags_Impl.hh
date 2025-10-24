#if !defined(EMBYFLAGS_FREERTOS_IMPL_H)
#define EMBYFLAGS_FREERTOS_IMPL_H

#include <EmbyThreading/Semaphore.hh>

namespace EmbyThreading
{
	struct FlagsImpl
	{
		EmbyThreading::Semaphore	 semaphore;
	};
}

#endif
