#include "mbed.h"
#include <EmbyThreading/Mutex.hh>
#include <EmbyTime/Millis.hh>
#include <EmbySystem/System.hh>


namespace EmbyThreading
{
    Mutex::Mutex(Type type)
    {

        (void) type;
    }

    void Mutex::release()
    {
        if (!EmbySystem::isInInterrupt() && EmbySystem::areInterruptsEnabled())
        {
            m_impl.mutex.unlock();
        }
    }

    void Mutex::acquire()
    {
        if (!EmbySystem::isInInterrupt() && EmbySystem::areInterruptsEnabled())
        {
            m_impl.mutex.lock();
        }
    }

    bool Mutex::tryAcquire(EmbyTime::Millis timeout)
    {
        if (!EmbySystem::isInInterrupt() && EmbySystem::areInterruptsEnabled())
        {
            return m_impl.mutex.trylock_for(timeout);
        }
        else
        {
            return false;
        }
    }
} // namespace EmbyThreading
