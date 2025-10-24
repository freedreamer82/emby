

#if !defined( EMBYTHREADING_SCOPEDMUTEX_HH )
#define EMBYTHREADING_SCOPEDMUTEX_HH

#include <EmbyThreading/Mutex.hh>

namespace EmbyThreading
{
    /** 
     *
     * The scoped mutex class provides a simple RAII (Resource Acquisition Is
     * Initialization) for dealing wiht mutex along a scope. The scope can be
     * considered a critical section that can be executed by no more than a
     * single thread. 
     *
     * RAII: http://www.hackcraft.net/raii/
     */

    class ScopedMutex
    {
        public:
            /**
             * Constructs a scoped mutex and locks the given mutex. If the
             * mutex cannot be locked, an exception is thrown.
             *
             * @param mux the mutex to lock.
             * @throws Mutex_Failure is thrown if the lock operation fails for
             *                       wathever reason.
             */
            explicit ScopedMutex( Mutex& mux );

            /**
             * The destructor automatically release the mutex. Errors are
             * ignored.
             */
            ~ScopedMutex();
        private:
            ScopedMutex( ScopedMutex const& copy );
            ScopedMutex& operator=( ScopedMutex const& copy );

            Mutex& m_mutex;
    };

    // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // :::::::::::::::::::::::::::::::::::::::::::::::::::: inline methods :::
    // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    inline
    ScopedMutex::ScopedMutex( Mutex& mux ) : m_mutex( mux )
    {
        m_mutex.acquire();
    }

    inline
    ScopedMutex::~ScopedMutex()
    {
        m_mutex.release();
    }

}  // end of namespace EmbyThreading


#endif
///@}


