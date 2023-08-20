#include <EmbyThreading/Thread.hh>
#include "EmbyDebug/assert.hh"

namespace EmbyThreading
{
	static void*
	launcher( void * param )
	{
		auto instance = reinterpret_cast<EmbyThreading::Thread*>( param );
		instance->run();
		return nullptr;
	}

    Thread::Thread( Worker* worker,
                    char const* name,
                    size_t stackSize,
                    Priority  priority , bool start,
                    uint32_t * stack )
	{
    	(void)stackSize;
    	(void)name;
    	(void)priority;
    	EmbyDebug_ASSERT_CHECK_NULL_PTR(worker);
		m_worker    = worker;
                m_isRunning = start;
		int retval = pthread_create( &m_impl.m_handle ,  nullptr, launcher , this);
		EmbyDebug_ASSERT( retval == 0 );
	}

    bool
    Thread::start()
    {
        m_isRunning = true;
	return true;    
    }

    bool
    Thread::pause()
    {
        m_isRunning = false;
	return true;    
    }


    void
    Thread::yield()
    {

    }

    void
    Thread::join()
    {
    	pthread_exit(&m_impl.m_handle);
    }

    void Thread::run()
    {
        while(1)
        {
            if (m_isRunning){
                m_worker->doWork();
            }
            else{
                EmbySystem::delayMs(100);
            }

        }
    }

}

