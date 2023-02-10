#include <EmbyThreading/Thread.hh>
#include <EmbyThreading/Worker.hh>
#include "EmbyDebug/assert.hh"

namespace EmbyThreading
{
	static void
	launcher( void * param )
	{
		auto instance = reinterpret_cast<EmbyThreading::Thread*>( param );
		instance->run();
	}

    Thread::Thread( EmbyThreading::Worker* worker,
    			    char const* name,
    			    size_t stackSize,
    			    int priority , bool start,uint32_t *stack)
	{

    	EmbyDebug_ASSERT_CHECK_NULL_PTR(worker);
		m_worker    = worker;
		m_isRunning = start;
		BaseType_t retval = xTaskCreate( launcher,
				 	 	 	 	 	 	 name,stackSize,
				 	 	 	 	 	 	 this,
				 	 	 	 	 	 	 priority,
				 	 	 	 	 	 	 &m_impl.m_handle );
		EmbyDebug_ASSERT( retval == pdPASS );
		EmbyDebug_ASSERT_CHECK_NULL_PTR(m_impl.m_handle);
	}

    bool
    Thread::start()
    {
    	m_isRunning = true;
    	return true;
    }

    void
    Thread::yield()
    {
        taskYIELD();
    }

    void Thread::run()
    {
        while(1)
        {
        	if(m_isRunning )
        	{
        		m_worker->doWork();
        	}
            yield();
            //EmbySystem::delayMs(10);
        }
    }

}

