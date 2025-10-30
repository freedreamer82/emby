#include <EmbyThreading/Thread.hh>
#include <EmbyThreading/Worker.hh>
#include "EmbyDebug/assert.hh"
#include "FreeRTOS.h"
#include "task.h"

namespace EmbyThreading
{
	static void
	launcher( void * param )
	{
		auto instance = reinterpret_cast<EmbyThreading::Thread*>( param );
		instance->run();
	}

    static int getPrio(EmbyThreading::Thread::Priority priority)
    {
        switch (priority)
        {
            case EmbyThreading::Thread::Priority::VeryHigh:
                return  configMAX_PRIORITIES - 1;
            case EmbyThreading::Thread::Priority::High:
                return  configMAX_PRIORITIES - 2;
            case EmbyThreading::Thread::Priority::Normal:
                return  configMAX_PRIORITIES - 3;
            case EmbyThreading::Thread::Priority::Low:
                return  configMAX_PRIORITIES - 4;
            case EmbyThreading::Thread::Priority::VeryLow:
                return  configMAX_PRIORITIES - 5;
        }

        return configMAX_PRIORITIES - 3;;
    }


    Thread::Thread( EmbyThreading::Worker* worker,
                    char const* name,
                    uint32_t stackSize,
                    Priority priority , bool start,uint32_t *stack)
	{

    	EmbyDebug_ASSERT_CHECK_NULL_PTR(worker);
		m_worker    = worker;
		m_isRunning = start;
		volatile BaseType_t retval = xTaskCreate( launcher,
				 	 	 	 	 	 	 name,stackSize,
				 	 	 	 	 	 	 this,
                                         getPrio(priority),
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
        }
    }

}

