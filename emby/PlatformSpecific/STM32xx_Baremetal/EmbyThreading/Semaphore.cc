
#include <EmbyThreading/Semaphore.hh>
#include <EmbySystem/System.hh>
#include <EmbyDebug/assert.hh>


namespace EmbyThreading {

    Semaphore::Semaphore() :
            m_impl{.sem = 0} {
    }

    Semaphore::~Semaphore() {

    }


    bool Semaphore::signal() {

    	m_impl.sem++;
        return true;

    }

    bool Semaphore::wait(EmbyTime::Millis timeout) {

    	bool retval = true;
    	auto start = EmbySystem::upTimeMs();

    	while(m_impl.sem ==0)
    	{
    		if( EmbySystem::upTimeMs() - start > timeout)
    		{
    			retval = false;
    			break;
    		}
    		EmbySystem::delayMs(1);
    	}

    	if (m_impl.sem !=0 )
    	{
        	m_impl.sem--;
    	}
        return retval;
    }

    bool Semaphore::isSomeoneWaiting() const {
        return false;
    }

} /* namespace EmbyThreading */
