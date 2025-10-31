#include <EmbyThreading/Thread.hh>
#include "EmbyDebug/assert.hh"
#include "mbed.h"

void EmbyThreading::Thread::yield()
{
    rtos::ThisThread::yield();
}

static osPriority getPrio(EmbyThreading::Thread::Priority priority)
{
    switch (priority)
    {
        case EmbyThreading::Thread::Priority::VeryHigh:
            return osPriorityHigh;
        case EmbyThreading::Thread::Priority::High:
            return osPriorityHigh7;
        case EmbyThreading::Thread::Priority::Normal:
            return osPriorityNormal;
        case EmbyThreading::Thread::Priority::Low:
            return osPriorityLow;
        case EmbyThreading::Thread::Priority::VeryLow:
            return osPriorityLow7;
    }

    return osPriorityNormal;
}



EmbyThreading::Thread::Thread(EmbyThreading::Worker* worker,
                              char const*            name,
                              uint32_t                 stackSize,
                              Priority               priority,
                              bool                   start,
                              uint32_t*              stack)
{
    EmbyDebug_ASSERT_CHECK_NULL_PTR(worker);

    m_impl.m_thread = new rtos::Thread(getPrio(priority), stackSize, nullptr, name);
    EmbyDebug_ASSERT_CHECK_NULL_PTR(m_impl.m_thread);
    m_worker = worker;
    if (start)
    {
        m_isRunning = true;
        m_impl.m_thread->start(mbed::callback(this, &EmbyThreading::Thread::run));

    }
}
namespace EmbyThreading
{
    void Thread::run()
    {
        while(m_isRunning)
        {
            m_worker->doWork();
        }
    }



    bool Thread::start()
    {
        if (!m_isRunning)
        {
            m_isRunning = true;
            m_impl.m_thread->start(mbed::callback(m_worker, &EmbyThreading::Worker::doWork));
        }
        else
        {
            return false;
        }
        return true;
    }

    void Thread::join()
    {
        m_impl.m_thread->terminate();
        do
        {
            EmbySystem::delayMs(1);
        } while (m_impl.m_thread->get_state() != m_impl.m_thread->Deleted);
    }
} // namespace EmbyThreading
