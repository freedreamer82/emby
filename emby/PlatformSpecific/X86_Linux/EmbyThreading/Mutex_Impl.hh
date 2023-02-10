#if !defined(EMBYMUTEX_POSIX_IMPL_H)
#define EMBYMUTEX_POSIX_IMPL_H

//#include <bits/pthreadtypes.h>
#include <pthread.h>

namespace EmbyThreading
{

	struct MutexImpl
	{
		pthread_mutex_t      m_mutex;
		pthread_mutexattr_t  m_attr;
	};

}

#endif
