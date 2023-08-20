#include <EmbyThreading/Mutex.hh>
#include <stdlib.h>
#include <stdint.h>
//#include <bits/pthreadtypes.h>
#include <pthread.h>

namespace EmbyThreading
{
    Mutex:: Mutex(Type type  )
	{
	    (void)type;
		pthread_mutexattr_init(&m_impl.m_attr);
		pthread_mutexattr_settype(&m_impl.m_attr, PTHREAD_MUTEX_RECURSIVE);
		int ok = pthread_mutex_init(&m_impl.m_mutex, &m_impl.m_attr);
		EmbyDebug_ASSERT(ok == 0);
	}

	void
	Mutex::release()
	{
		pthread_mutex_unlock(&m_impl.m_mutex);
	}

	void
	Mutex::acquire()
	{
		pthread_mutex_lock(&m_impl.m_mutex);
	}
}
