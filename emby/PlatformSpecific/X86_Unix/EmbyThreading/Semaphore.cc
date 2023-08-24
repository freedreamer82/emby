#include <EmbyThreading/Semaphore.hh>
#include <EmbySystem/System.hh>
#include <errno.h>
#include <semaphore.h>
#include <time.h>

#ifdef __APPLE__
#include "pthread.h"
typedef struct
{
    pthread_mutex_t count_lock;
    pthread_cond_t  count_bump;
    unsigned count;
}
bosal_sem_t;

int sem_init(sem_t *psem, int flags, unsigned count)
{
    bosal_sem_t *pnewsem;
    int result;

    pnewsem = (bosal_sem_t *)malloc(sizeof(bosal_sem_t));
    if (! pnewsem)
    {
        return -1;
    }
    result = pthread_mutex_init(&pnewsem->count_lock, NULL);
    if (result)
    {
        free(pnewsem);
        return result;
    }
    result = pthread_cond_init(&pnewsem->count_bump, NULL);
    if (result)
    {
        pthread_mutex_destroy(&pnewsem->count_lock);
        free(pnewsem);
        return result;
    }
    pnewsem->count = count;
    *psem = (uint64_t)pnewsem;
    return 0;
}

int sem_destroy(sem_t *psem)
{
    bosal_sem_t *poldsem;

    if (! psem)
    {
        return EINVAL;
    }
    poldsem = (bosal_sem_t *)*psem;

    pthread_mutex_destroy(&poldsem->count_lock);
    pthread_cond_destroy(&poldsem->count_bump);
    free(poldsem);
    return 0;
}

int sem_post(sem_t *psem)
{
     bosal_sem_t *pxsem;
    int result, xresult;

    if (! psem)
    {
        return EINVAL;
    }
    pxsem = (bosal_sem_t *)*psem;

    result = pthread_mutex_lock(&pxsem->count_lock);
    if (result)
    {
        return result;
    }
    pxsem->count = pxsem->count + 1;

    xresult = pthread_cond_signal(&pxsem->count_bump);

    result = pthread_mutex_unlock(&pxsem->count_lock);
    if (result)
    {
        return result;
    }
    if (xresult)
    {
        errno = xresult;
        return -1;
    }
     return 0;
}

int sem_trywait(sem_t *psem)
{
    bosal_sem_t *pxsem;
    int result, xresult;

    if (! psem)
    {
        return EINVAL;
    }
    pxsem = (bosal_sem_t *)*psem;

    result = pthread_mutex_lock(&pxsem->count_lock);
    if (result)
    {
        return result;
    }
    xresult = 0;

    if (pxsem->count > 0)
    {
        pxsem->count--;
    }
    else
    {
        xresult = EAGAIN;
    }
    result = pthread_mutex_unlock(&pxsem->count_lock);
    if (result)
    {
        return result;
    }
    if (xresult)
    {
        errno = xresult;
        return -1;
    }
    return 0;
}

int sem_wait(sem_t *psem)
{
    bosal_sem_t *pxsem;
    int result, xresult;

    if (! psem)
    {
        return EINVAL;
    }
    pxsem = (bosal_sem_t *)*psem;

    result = pthread_mutex_lock(&pxsem->count_lock);
    if (result)
    {
        return result;
    }
    xresult = 0;

    if (pxsem->count == 0)
    {
        xresult = pthread_cond_wait(&pxsem->count_bump, &pxsem->count_lock);
    }
    if (! xresult)
    {
        if (pxsem->count > 0)
        {
            pxsem->count--;
        }
    }
    result = pthread_mutex_unlock(&pxsem->count_lock);
    if (result)
    {
        return result;
    }
    if (xresult)
    {
        errno = xresult;
        return -1;
    }
    return 0;
}

int sem_timedwait(sem_t *psem, const struct timespec *abstim)
{
    bosal_sem_t *pxsem;
    int result, xresult;

    if (! psem)
    {
        return EINVAL;
    }
    pxsem = (bosal_sem_t *)*psem;

    result = pthread_mutex_lock(&pxsem->count_lock);
    if (result)
    {
        return result;
    }
    xresult = 0;

    if (pxsem->count == 0)
    {
        xresult = pthread_cond_timedwait(&pxsem->count_bump, &pxsem->count_lock, abstim);
    }
    if (! xresult)
    {
        if (pxsem->count > 0)
        {
            pxsem->count--;
        }
    }
    result = pthread_mutex_unlock(&pxsem->count_lock);
    if (result)
    {
        return result;
    }
    if (xresult)
    {
        errno = xresult;
        return -1;
    }
    return 0;
}

#endif



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
