/**
 * @addtogroup EmbyMutex
 * @{
 * @file EmbyMutex/EmbyMutex.hh
 * @author Garzola Marco
 * @version 1.0
 * @date 11/10/2014
 *
 */

#if !defined( EMBY_EMBYMUTEX_HH )
#define EMBY_EMBYMUTEX_HH

//#include <cstdlib>
#include <EmbyDebug/assert.hh>
#include <EmbyThreading/Mutex_Impl.hh>
#include <EmbyTime/Millis.hh>

namespace EmbyThreading
{
	/*Mutex interface*/

    class Mutex
	{
		public:
        enum class Type
        {
            Recursive,
            NonRecursive
        };

		Mutex(Type type = Type::Recursive);

        ~Mutex() = default;
		/**
		 * Acquire Mutex lock
		 *
		 */
		void
		acquire();

		/**
		 * Release Mutex lock
		 *
		 */
		void
		release();

		/**
		 * Try Acquire a mutex
		 * */
		bool
		tryAcquire(EmbyTime::Millis timeout = 0);

		private:
			 MutexImpl m_impl;
	};

}

#endif



