#include <EmbyThreading/Semaphore.hh>
#include <EmbySystem/System.hh>
#include <EmbyDebug/assert.hh>


namespace EmbyThreading
{

    Semaphore::Semaphore() :
            m_impl{}
    {
        m_impl.isWaitingSignal = false;
    }

    Semaphore::~Semaphore()
    {

    }

    bool Semaphore::signal()
    {
        return m_impl.sem.release() == 0;
    }

    bool Semaphore::wait(EmbyTime::Millis timeout)
    {
        m_impl.isWaitingSignal = true;
        bool isTaken = m_impl.sem.try_acquire_for(timeout);
        m_impl.isWaitingSignal = false;
        return isTaken;
    }

    bool Semaphore::isSomeoneWaiting() const
    {
        return m_impl.isWaitingSignal;
    }

} /* namespace EmbyThreading */
