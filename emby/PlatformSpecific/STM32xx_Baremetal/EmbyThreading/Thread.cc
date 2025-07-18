#include <EmbyThreading/Thread.hh>
#include <EmbyThreading/Worker.hh>
#include "EmbyDebug/assert.hh"
namespace EmbyThreading
{


    Thread::Thread( EmbyThreading::Worker* worker,
    			    char const* name,
    			    size_t stackSize,
                    Priority priority , bool start,uint32_t *stack): m_worker{worker}
	{


	}

    bool
    Thread::start()
    {

    	m_worker->doWork();
    	return false;
    }

    void
    Thread::yield()
    {

    }

    void Thread::run()
    {

    }

}

