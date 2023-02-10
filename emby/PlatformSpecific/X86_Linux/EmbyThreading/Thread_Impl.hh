#if !defined(EMBYTHREAD_FREERTOS_IMPL_H)
#define EMBYTHREAD_FREERTOS_IMPL_H

#include "pthread.h"
#include <sys/time.h>
#include "pthread.h"
#include "unistd.h"
//#include  <linux/ioctl.h>
#include <fcntl.h>

namespace EmbyThreading
{
    static int constexpr PRIORITY_LOWEST  =  -1;

	struct ThreadImpl
	{
		 pthread_t  m_handle;
	};
}

#endif
