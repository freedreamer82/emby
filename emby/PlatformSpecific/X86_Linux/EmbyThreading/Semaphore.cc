/**
 * @addtogroup EmbyThreading
 * @{
 * @file EmbyThreading/Semaphore.cc
 * @author apagani
 * @version 1.0
 * @date Jan 28, 2015
 */

#include <EmbyThreading/Semaphore.hh>
#include <EmbySystem/System.hh>
#include <time.h>
#include <errno.h>

namespace EmbyThreading
{

    Semaphore::Semaphore()
    {
        sem_init( &m_impl, 0, 0 );
    }

    Semaphore::~Semaphore()
    {
        sem_destroy( &m_impl );
    }


    bool Semaphore::signal()
    {
        int value;
        sem_getvalue( &m_impl, &value );
        sem_post( &m_impl );
        return value == 0;
    }

    bool Semaphore::wait( EmbyTime::Millis timeout )
    {
        long timeout_ms = timeout % 1000u;
        long timeout_s = timeout / 1000u;

        struct timespec alarmTime;
        clock_gettime( CLOCK_REALTIME, &alarmTime );
        alarmTime.tv_sec += timeout_s;
        alarmTime.tv_nsec += timeout_ms * 1000000;
        long carry = alarmTime.tv_nsec / 1000000000;
        if( carry != 0 )
        {
            alarmTime.tv_nsec %= 1000000000;
            alarmTime.tv_sec += carry;
        }
        int result;
        do
        {
            result = sem_timedwait( &m_impl, &alarmTime );
        }
        while( result == EINTR );
        if( result == ETIMEDOUT )
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    void Semaphore::signalFromInterrupt()
    {

    }

    bool Semaphore::isSomeoneWaiting() const
    {
        int value;
        sem_getvalue( const_cast<sem_t*>(&m_impl), &value );
        return value == 0;
    }

} /* namespace EmbyThreading */
