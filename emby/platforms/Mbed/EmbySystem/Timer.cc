#include <EmbySystem/Timer.hh>
// #include <critical.h>
#include "EmbyDebug/assert.hh"
#include "Timer_Impl.hh"

static uint16_t s_timerInstancesCount = 0;

using namespace EmbySystem;

//=============================================================================

namespace
{
    uint16_t constexpr THREAD_FLAG_TICK = 1 << 0; // Software timer flag
    uint16_t constexpr THREAD_FLAG_STOP = 1 << 1; // Thread refresh
} // namespace

void TimerImpl::threadCallback()
{
    while (running)
    {
        uint32_t flags = osThreadFlagsWait(THREAD_FLAG_TICK, osFlagsWaitAny, osWaitForever);

        if (flags & THREAD_FLAG_STOP)
        {
            running = false;
        }
        else if (flags & THREAD_FLAG_TICK)
        {
            if (timer)
            {
                timer->callback(*timer);

                // If it's not a periodic timer,
                // just stop after the first tick.
                if (!periodic)

                {
                    running = false;
                }
            }
        }
    }
}

void TimerImpl::tickerCallback()
{
    if (!running)
    {
        return; // If not running, do nothing
    }

    // If SW timer
    if (thread != nullptr)
    {
        osThreadFlagsSet(thread->get_id(), THREAD_FLAG_TICK);
    }
    else
    {
        if (timer)
        {
            // Manually call the callback from ISR
            timer->callback(*timer);
        }
    }
}

//=============================================================================

EmbySystem::Timer::Timer(EmbySystem::TimerSize ms, bool isPeriodic, uint32_t* stack, Type type)

{
//    core_util_atomic_incr_u16(&s_timerInstancesCount, 1);
//
//    m_impl.timer = this;
//
//    if (type == Type::Software)
//    {
//
//       m_impl.threadName = nullptr;
//
//
//        m_impl.thread = new Thread{osPriorityNormal,
//                                   static_cast<uint32_t>(stackSize),
//                                   reinterpret_cast<unsigned char*>(stack),
//                                   m_impl.threadName};
//        EmbyDebug_ASSERT_CHECK_NULL_PTR(m_impl.thread);
//    }
//    else
//    {
//        m_impl.thread = nullptr;
//    }
//
//    m_impl.periodic   = isPeriodic;
//    m_impl.running    = false;
//    m_impl.durationMs = ms;
}

//=============================================================================

EmbySystem::Timer::~Timer()
{
    // Stop the timer
    stop();

    // Delete the thread if it's a SW timer
    if (m_impl.thread != nullptr)
    {
        delete m_impl.thread;
    }

    if (m_impl.threadName != nullptr)
    {
        delete[] m_impl.threadName;
    }

    core_util_atomic_decr_u16(&s_timerInstancesCount, 1);
}

//=============================================================================

bool EmbySystem::Timer::isActive()
{
    return m_impl.running;
}

//=============================================================================

bool EmbySystem::Timer::start(EmbySystem::TimerSize)
{
    // If already running return true
    if (m_impl.running)
    {
        return true;
    }

    m_impl.running = true;

    // If the timer is SW, start the thread
    if (m_impl.thread)
    {
        if (m_impl.thread->start(mbed::callback(&m_impl, &TimerImpl::threadCallback)) != osOK)
        {
            m_impl.running = false;
            return false;
        }
    }

    m_impl.ticker.attach_us(mbed::callback(&m_impl, &TimerImpl::tickerCallback),
                            m_impl.durationMs * 1000);

    return true;
}

//=============================================================================

bool EmbySystem::Timer::stop(EmbySystem::TimerSize)
{
    if (!m_impl.running)
    {
        // Timer already stopped
        return false;
    }

    // Stop the ticker
    m_impl.ticker.detach();

    // If this is a software timer
    // stop the thread
    if (m_impl.thread)
    {
        osThreadFlagsSet(m_impl.thread->get_id(), THREAD_FLAG_STOP);
        return m_impl.thread->join() == osOK;
    }
    else
    {
        m_impl.running = false;
        return true;
    }
}

//=============================================================================

bool EmbySystem::Timer::reset(EmbySystem::TimerSize)
{
    if (m_impl.running)
    {
        m_impl.ticker.detach();
        m_impl.ticker.attach_us(mbed::callback(&m_impl, &TimerImpl::tickerCallback),
                                m_impl.durationMs * 1000);
    }

    return true;
}

//=============================================================================

bool EmbySystem::Timer::setPeriod(EmbySystem::TimerSize NewPeriod, EmbySystem::TimerSize)
{
    m_impl.durationMs = NewPeriod;

    // If running, restart the ticker
    if (m_impl.running)
    {
        m_impl.ticker.detach();
        m_impl.ticker.attach_us(mbed::callback(&m_impl, &TimerImpl::tickerCallback),
                                m_impl.durationMs * 1000);
    }

    return true;
}

//=============================================================================

//
// #include <EmbySystem/Timer.hh>
// #include <critical.h>
// #include "EmbyDebug/assert.hh"
//
// static uint16_t s_timerInstancesCount = 0;
//
// using namespace EmbySystem;
//
////=============================================================================
//
// namespace
//{
//    uint16_t constexpr THREAD_FLAG_TICK = 1 << 0; // Software timer flag
//    uint16_t constexpr THREAD_FLAG_STOP = 1 << 1; // Thread refresh
//} // namespace
//
// void TimerImpl::threadCallback()
//{
//    while (running)
//    {
//        uint32_t flags = osThreadFlagsWait(THREAD_FLAG_TICK, osFlagsWaitAny, osWaitForever);
//
//        if (flags & THREAD_FLAG_STOP)
//        {
//            running = false;
//        }
//        else if (flags & THREAD_FLAG_TICK)
//        {
//            if (timer)
//            {
//                timer->callback(*timer);
//
//                // If it's not a periodic timer,
//                // just stop after the first tick.
//                if (!periodic)
//
//
//
//                {
//                    running = false;
//                }
//            }
//        }
//    }
//}
//
// void TimerImpl::tickerCallback()
//{
//    if (!running)
//    {
//        return; // If not running, do nothing
//    }
//
//    // If SW timer
//    if (thread != nullptr)
//    {
//        osThreadFlagsSet(thread->get_id(), THREAD_FLAG_TICK);
//    }
//    else
//    {
//        if (timer)
//        {
//            // Manually call the callback from ISR
//            timer->callback(*timer);
//        }
//    }
//}
//
////=============================================================================
//
// EmbySystem::Timer::Timer(EmbySystem::TimerSize   ms,
//                         bool                    isPeriodic,
//                         EmbySystem::Timer::Type type,
//                         size_t                  stackSize,
//                         const char*             name,
//                         uint32_t*               stack)
//{
//    core_util_atomic_incr_u16(&s_timerInstancesCount, 1);
//
//    m_impl.timer = this;
//
//    if (type == Type::Software)
//    {
//        if (name == nullptr)
//        {
//            static size_t constexpr NAME_LEN = 11; // Space for "timer-"+[0-65535]
//            m_impl.threadName                = new char[NAME_LEN + 1];
//            std::snprintf(m_impl.threadName, NAME_LEN, "timer-%u", s_timerInstancesCount);
//            m_impl.threadName[NAME_LEN] = '\0';
//        }
//        else
//        {
//            m_impl.threadName = nullptr;
//        }
//
//        m_impl.thread = new Thread{osPriorityNormal,
//                                   static_cast<uint32_t>(stackSize),
//                                   reinterpret_cast<unsigned char*>(stack),
//                                   m_impl.threadName};
//        EmbyDebug_ASSERT_CHECK_NULL_PTR(m_impl.thread);
//
//    }
//    else
//    {
//        m_impl.thread = nullptr;
//    }
//
//    m_impl.periodic   = isPeriodic;
//    m_impl.running    = false;
//    m_impl.durationMs = ms;
//}
//
////=============================================================================
//
// EmbySystem::Timer::~Timer()
//{
//    // Stop the timer
//    stop();
//
//    // Delete the thread if it's a SW timer
//    if (m_impl.thread != nullptr)
//    {
//        delete m_impl.thread;
//    }
//
//    if (m_impl.threadName != nullptr)
//    {
//        delete[] m_impl.threadName;
//    }
//
//    core_util_atomic_decr_u16(&s_timerInstancesCount, 1);
//}
//
////=============================================================================
//
// bool EmbySystem::Timer::isActive()
//{
//    return m_impl.running;
//}
//
////=============================================================================
//
// bool EmbySystem::Timer::start(EmbySystem::TimerSize)
//{
//    // If already running return true
//    if (m_impl.running)
//    {
//        return true;
//    }
//
//    m_impl.running = true;
//
//    // If the timer is SW, start the thread
//    if (m_impl.thread)
//    {
//        if (m_impl.thread->start(mbed::callback(&m_impl, &TimerImpl::threadCallback)) != osOK)
//        {
//            m_impl.running = false;
//            return false;
//        }
//    }
//
//    m_impl.ticker.attach_us(mbed::callback(&m_impl, &TimerImpl::tickerCallback),
//                            m_impl.durationMs * 1000);
//
//    return true;
//}
//
////=============================================================================
//
// bool EmbySystem::Timer::stop(EmbySystem::TimerSize)
//{
//    if (!m_impl.running)
//    {
//        // Timer already stopped
//        return false;
//    }
//
//    // Stop the ticker
//    m_impl.ticker.detach();
//
//    // If this is a software timer
//    // stop the thread
//    if (m_impl.thread)
//    {
//        osThreadFlagsSet(m_impl.thread->get_id(), THREAD_FLAG_STOP);
//        return m_impl.thread->join() == osOK;
//    }
//    else
//    {
//        m_impl.running = false;
//        return true;
//    }
//}
//
////=============================================================================
//
// bool EmbySystem::Timer::reset(EmbySystem::TimerSize)
//{
//    if (m_impl.running)
//    {
//        m_impl.ticker.detach();
//        m_impl.ticker.attach_us(mbed::callback(&m_impl, &TimerImpl::tickerCallback),
//                                m_impl.durationMs * 1000);
//    }
//
//    return true;
//}
//
////=============================================================================
//
// bool EmbySystem::Timer::setPeriod(EmbySystem::TimerSize NewPeriod, EmbySystem::TimerSize)
//{
//    m_impl.durationMs = NewPeriod;
//
//    // If running, restart the ticker
//    if (m_impl.running)
//    {
//        m_impl.ticker.detach();
//        m_impl.ticker.attach_us(mbed::callback(&m_impl, &TimerImpl::tickerCallback),
//                                m_impl.durationMs * 1000);
//    }
//
//    return true;
//}
//
////=============================================================================
//
//
//
//
//
////
//
//
//
//
//
//
// #include <EmbySystem/Timer.hh>
// #include <critical.h>
// #include "EmbyDebug/assert.hh"
//
// static uint16_t s_timerInstancesCount = 0;
// static constexpr uint16_t NUM_TIMER = 6;
//
//
//
//
//__attribute__((section(".itcmram"), used)) uint8_t s_buffer[EVENTS_EVENT_SIZE*NUM_TIMER];
// using namespace EmbySystem;
//
//
// static EventQueue s_eventsQueue(EVENTS_EVENT_SIZE*NUM_TIMER,s_buffer );
// static Thread s_thread;
//
//
////=============================================================================
//
// namespace
//{
//    uint16_t constexpr THREAD_FLAG_TICK = 1 << 0; // Software timer flag
//    uint16_t constexpr THREAD_FLAG_STOP = 1 << 1; // Thread refresh
//} // namespace
//
//
//
////=============================================================================
//
// EmbySystem::Timer::Timer(EmbySystem::TimerSize   ms,
//                         bool                    isPeriodic,
//                         EmbySystem::Timer::Type type,
//                         size_t                  stackSize,
//                         const char*             name,
//                         uint32_t*               stack)
//{
//    core_util_atomic_incr_u16(&s_timerInstancesCount, 1);
//    s_thread.start(::callback(&s_eventsQueue, &EventQueue::dispatch_forever));
//    m_impl.timer = this;
//
//    if (type == Type::Software)
//    {
//        if (name == nullptr)
//        {
//            static size_t constexpr NAME_LEN = 11; // Space for "timer-"+[0-65535]
//            m_impl.threadName                = new char[NAME_LEN + 1];
//            std::snprintf(m_impl.threadName, NAME_LEN, "timer-%u", s_timerInstancesCount);
//            m_impl.threadName[NAME_LEN] = '\0';
//        }
//        else
//        {
//            m_impl.threadName = nullptr;
//        }
//
//        m_impl.thread = new Thread{osPriorityNormal,
//                                   static_cast<uint32_t>(stackSize),
//                                   reinterpret_cast<unsigned char*>(stack),
//                                   m_impl.threadName};
//        EmbyDebug_ASSERT_CHECK_NULL_PTR(m_impl.thread);
//
//    }
//    else
//    {
//        m_impl.thread = nullptr;
//    }
//
//    m_impl.periodic   = isPeriodic;
//    m_impl.running    = false;
//    m_impl.durationMs = ms;
//}
//
////=============================================================================
//
// EmbySystem::Timer::~Timer()
//{
//    // Stop the timer
//    stop();
//
//    // Delete the thread if it's a SW timer
//    if (m_impl.thread != nullptr)
//    {
//        delete m_impl.thread;
//    }
//
//    if (m_impl.threadName != nullptr)
//    {
//        delete[] m_impl.threadName;
//    }
//
//    core_util_atomic_decr_u16(&s_timerInstancesCount, 1);
//}
//
////=============================================================================
//
// bool EmbySystem::Timer::isActive()
//{
//    return m_impl.running;
//}
//
////=============================================================================
//
// bool EmbySystem::Timer::start(EmbySystem::TimerSize)
//{
//    // If already running return true
//    if (m_impl.running)
//    {
//        return true;
//    }
//
//    m_impl.running = true;
//
//    // If the timer is SW, start the thread
//    if (m_impl.thread)
//    {
//        if (m_impl.thread->start(mbed::callback(&m_impl, &TimerImpl::threadCallback)) != osOK)
//        {
//            m_impl.running = false;
//            return false;
//        }
//    }
//
//    m_impl.ticker.attach_us(mbed::callback(&m_impl, &TimerImpl::tickerCallback),
//                            m_impl.durationMs * 1000);
//
//    return true;
//}
//
////=============================================================================
//
// bool EmbySystem::Timer::stop(EmbySystem::TimerSize)
//{
//    if (!m_impl.running)
//    {
//        // Timer already stopped
//        return false;
//    }
//
//    // Stop the ticker
//    m_impl.ticker.detach();
//
//    // If this is a software timer
//    // stop the thread
//    if (m_impl.thread)
//    {
//        osThreadFlagsSet(m_impl.thread->get_id(), THREAD_FLAG_STOP);
//        return m_impl.thread->join() == osOK;
//    }
//    else
//    {
//        m_impl.running = false;
//        return true;
//    }
//}
//
////=============================================================================
//
// bool EmbySystem::Timer::reset(EmbySystem::TimerSize)
//{
//    if (m_impl.running)
//    {
//        m_impl.ticker.detach();
//        m_impl.ticker.attach_us(mbed::callback(&m_impl, &TimerImpl::tickerCallback),
//                                m_impl.durationMs * 1000);
//    }
//
//    return true;
//}
//
////=============================================================================
//
// bool EmbySystem::Timer::setPeriod(EmbySystem::TimerSize NewPeriod, EmbySystem::TimerSize)
//{
//    m_impl.durationMs = NewPeriod;
//
//    // If running, restart the ticker
//    if (m_impl.running)
//    {
//        m_impl.ticker.detach();
//        m_impl.ticker.attach_us(mbed::callback(&m_impl, &TimerImpl::tickerCallback),
//                                m_impl.durationMs * 1000);
//    }
//
//    return true;
//}
//
////=============================================================================
