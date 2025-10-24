
#if !defined( EMBY_EMBYTHREAD_HH )
#define EMBY_EMBYTHREAD_HH

#include  "Thread_Impl.hh"
#include <EmbyThreading/Worker.hh>
#include <stdint.h>
#include <cstddef>

namespace EmbyThreading
{

    /*Thread interface*/
    class Thread
    {
    public:
        enum class Priority: uint8_t
        {
            VeryHigh,
            High,
            Normal,
            Low,
            VeryLow,
        };

        Thread(EmbyThreading::Worker *worker,
               char const *name,
               size_t stackSize,
               Priority priority, bool start = true,
               uint32_t *stack = nullptr);

        /**
         * Acquire Mutex lock
         *
         */
        bool
        start();

        bool
        pause();

        /**
         * Wait sync util thread is over
         *
         */
        void
        join();

        /**
         * check if thread i sRunning;
         */
        bool
        isRunning() const;

        /**
         * Yield the control back to the OS allowing another thread to
         * run.
         */
        static void yield();

        void run();

    protected:
        volatile bool m_isRunning;
        ThreadImpl m_impl;

    private:
        Worker *m_worker;

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // ::::::::::::::::::::::::::::::::::::::::::::::::::::: Inline methods :::
    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    inline bool Thread::isRunning() const
    {
        return m_isRunning;
    }

}

#endif



