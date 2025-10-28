/**
 * @addtogroup EmbySystem
 * @{
 * @file EmbySystem/EmbySystem.hh
 * @author Emby Team
 * @version 1.0
 * @date 6/2017
 *
 */

#if !defined( EMBY_EMBYSYSTEM_HH )
#define EMBY_EMBYSYSTEM_HH

#include <cstdlib>
#include <EmbyTime/Millis.hh>
#include <EmbyDebug/assert.hh>
#include <limits>
#include "System_Impl.hh"

/**
 *
 * This file contains a number of basic functions. This functions are the most
 * basic functions that abstract from the specific architecture. 
 *
 */

namespace EmbySystem
{

    void EnterCriticalSection();

    void ExitCriticalSection();


    /**
     * Terminates the program execution. If the program is executed in the
     * debugger, the debugger is halted on this line.
     */

    void abort();


    bool isSchedulerRunning();


    /**
     * Returns the system time a (mostly) monotonically increasing number that
     * counts milliseconds.
     * 
     * @note after about 50 days of run-time this time will wrap around starting
     *       anew from 0. Take care of this.
     * 
     * @return the number of milliseconds from boot.
     */
    EmbyTime::Millis upTimeMs();


    /**
    * start the OS Scheduler
    */
    bool
    startScheduler();

    /**
    * check if is interrupt context
    */
    bool
    isInInterrupt();

    bool areInterruptsEnabled();

    /**
    * stop the OS Scheduler
    */
    bool
    stopScheduler();


    void go2sleep();

    bool isCoreSleeping();

    /**
    * delay this thread for at least ms millisecond.
    */
    void
    delayMs(EmbyTime::Millis ms);

    /**
     * Returns an unique id for the system.
     *
     * @return identifier unique to this board/processor.
     */
    SystemUniqueId
    getSystemUniqueId();

    /**
     * Critical section class is an exception safe way of dealing with critical section.
     * Instantiate an object of this class to enter the critical section. The critical
     * section is left when the scope is left or when an exception is thrown.
     */
    class CriticalSection
    {
    public:
        CriticalSection()
        {
            EnterCriticalSection();
        };

        ~CriticalSection()
        {
            ExitCriticalSection();
        };
    private:

    };


    void reboot(uint32_t delayMS = 0);

    /**
     * Converts milliseconds into OS ticks.
     */
    uint32_t
    millisToTicks(EmbyTime::Millis ms);

    /**
     * Converts OS ticks into milliseconds
     */
    EmbyTime::Millis
    ticksToMillis(uint32_t ticks);

}

#endif
///@}
