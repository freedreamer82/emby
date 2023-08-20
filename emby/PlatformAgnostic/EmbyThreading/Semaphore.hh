
#ifndef EMBY_THREADING_SEMAPHORE_HH_
#define EMBY_THREADING_SEMAPHORE_HH_

#include <EmbyTime/Millis.hh>
#include <EmbyThreading/Semaphore_Impl.hh>

namespace EmbyThreading
{

    class Semaphore
    {
        public:
            Semaphore();
            ~Semaphore();

            /**
             * Signals the semaphore.
             *
             * @retval true someone has been signaled.
             * @retval false no one was waiting for the signal.
             */
            bool signal();
            /**
             * Attempts to take the semaphore, that is waits for someone that
             * gives the semaphore.
             *
             * @param timeout for how many ms to wait before giving up.
             * @retval true the semaphore has been given.
             * @retavl false the timeout expired before someone gave the
             *               semaphore.
             */
            bool wait( EmbyTime::Millis timeout );
            void signalFromInterrupt();

            bool isSomeoneWaiting() const;
        private:
            SemaphoreImpl m_impl;
    };

} /* namespace EmbyThreading */

#endif /* SEMAPHORE_HH_ */
