#if !defined( EMBYMACHINE_WATCHDOG_HH_ )
#define EMBYMACHINE_WATCHDOG_HH_

#include <EmbyTime/Millis.hh>

namespace EmbyMachine
{

    namespace WatchDog
    {
        /**
         * This method sets the watchdog up. After setup the watchdog is disabled.
         *
         * @param timeout the maximum time you are allowed get along without a
         *                kick before the watchdog resets the system.
         */
        void setUp( EmbyTime::Millis timeout );

        /**
         * Enable the watchdog. Call this method to enable the watchdog.
         *
         * @retval true the watchdog has been properly enabled.
         * @retval false the watchdog failed to enable.
         */
        bool enable();

        /**
         * Disable the watchdog. Call this method to temporarily disable the
         * watchdog. Though note that watchfdog is there for a reason and
         * disabling it is usually not a good idea.
         *
         * @retval true the watchdog has been properly disabled.
         * @retval false the watchdog failed to disable.
         */
        bool disable();

        /**
         * watchdog kicking function. Call this function periodically, waiting
         * no longer than 'timeout' as defined in the setup, to avoid the
         * watchdog kicks you out of the system.
         */
        void kick();

        /**
         *
         */
        bool isRunning();
    };

} /* namespace EmbyMachine */

#endif /* WATCHDOG_HH_ */
