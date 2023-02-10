#include <EmbyThreading/Flags.hh>
#include "task.h"
#include <EmbySystem/System.hh>

namespace EmbyThreading
{


    Flags:: Flags() :m_flags(0) , m_impl()
    {
    }

	Flags::~Flags()
	{

	}

	uint32_t
	Flags::wait(uint32_t maskEventToWaitfor , int32_t timeoutMs)
	{
		auto retval = m_impl.semaphore.wait(timeoutMs);
		if( !retval )
		{
			return 0;
		}
		else
		{
			uint32_t mask;
            EmbySystem::EnterCriticalSection();
			mask = m_flags & maskEventToWaitfor;
			//clear mask
			m_flags = m_flags & ~mask;
            EmbySystem::ExitCriticalSection();

			return mask;
		}

	}

	bool
	Flags::post(uint32_t eventMask )
	{
		if ( EmbySystem::isInInterrupt() )
		{
			m_flags |= eventMask;
		}
		else
		{
            EmbySystem::EnterCriticalSection();
			m_flags |= eventMask;
            EmbySystem::ExitCriticalSection();
		}
		return m_impl.semaphore.signal();
	}
}
