/**
 * @addtogroup EmbySystem
 * @{
 * @file x86_Linux/EmbySystem/System.cc
 * @author Massimiliano Pagani
 * @version 1.0
 * @date 17/10/2014
 *
 */

#include <cstdlib>
#include <EmbySystem/System.hh>
#include <EmbyDebug/assert.hh>
#include <unistd.h>
#include <limits>
#include <time.h>
#include <sys/sysinfo.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <algorithm>


static bool g_isRunning = false;
namespace EmbySystem
{
    using EmbyTime::Millis;

	void abort()
	{
		::abort();
	}


	Millis upTimeMs()
	{
		__syscall_slong_t const NSEC_TO_MILLIS_DENOMINATOR = 1000*1000;
		timespec result;
		clock_gettime( CLOCK_MONOTONIC, &result );

		Millis time = result.tv_sec*1000LL +
					  result.tv_nsec/NSEC_TO_MILLIS_DENOMINATOR;
		return time;
	}

	bool
	waitInterrupt( Millis timeout )
	{
		EmbyDebug_ASSERT( std::numeric_limits<useconds_t>::max() / 1000 < timeout );
		useconds_t time;
		time = timeout*1000;
		usleep( time);
		return false;
	}



	void
	startScheduler(void)
	{
		g_isRunning = true;
		while(g_isRunning)
		{
			sleep(2);
		}
	}

	void
	stopScheduler(void)
	{
		g_isRunning = false;
	}

	void
	delayMs(Millis ms)
	{
		  usleep(ms*1000);
	}

    SystemUniqueId
    getSystemUniqueId()
    {
        SystemUniqueId id;
        std::fill( std::begin( id.m_id ), std::end( id.m_id ), 0 );
        return id;
    }

    void reboot(uint32_t delayMS)
    {
        EmbySystem::delayMs(delayMS);
        exit(0);
    }


	bool
	isInInterrupt(void)
	{
		return false;
	}
}

///@}


