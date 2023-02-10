/*
 * Timer.hh
 *
 *  Created on: 28/feb/2017
 *      Author: mgarzola
 */

#ifndef TIMER_HH_
#define TIMER_HH_


#include <EmbyLibs/Callback.hh>
#include <EmbySystem/TimerImpl.hh>
#include <exception>
#include <string>
#include <cstdio>


namespace EmbySystem
{
    typedef uint64_t TimerSize;

    class Timer : public EmbyLibs::Callback<void, Timer &>
    {
    public:

        enum class Type : uint8_t
        {
            Software,  // SW based
            Hardware   // based on ISR
        };

                /**
        *  Construct a named timer.
        *  Timers are not active after they are created, you need to
        *  activate them via Start, Reset, etc.
        *
        *  @param PeriodMss When does the timer expire and run your Run()
        *         method.
        *  @param Periodic true if the timer expires every PeriodInTicks.
        *         false if this is a one shot timer.
        */
        Timer(TimerSize ms, bool isPeriodic = true,Type = Type::Software);

        /**
         *  Construct a named timer.
         *  Timers are not active after they are created, you need to
         *  activate them via Start, Reset, etc.
         *
         *  @param PeriodMss When does the timer expire and run your Run()
         *         method.
         *  @param Periodic true if the timer expires every PeriodInTicks.
         *         false if this is a one shot timer.
         *  @param stack If not null, the timer will be allocated on the given static memory.
         */
        Timer(TimerSize ms, bool isPeriodic = true, uint32_t *stack = nullptr, Type = Type::Software);

        /**
         *  Destructor
         */
        ~Timer();

        /**
         *  Is the timer currently active?
         *
         *  @return true if the timer is active, false otherwise.
         */
        bool isActive();

        /**
         *  Start a timer. This changes the state to active.
         *
         *  @param CmdTimeout How long to wait to send this command to the
         *         timer code.
         *  @returns true if this command will be sent to the timer code,
         *           false if it will not (i.e. timeout).
         */
        bool start(TimerSize CmdTimeoutMs = 0);

        /**
         *  Stop a timer. This changes the state to inactive.
         *
         *  @param CmdTimeout How long to wait to send this command to the
         *         timer code.
         *  @returns true if this command will be sent to the timer code,
         *           false if it will not (i.e. timeout).
         */
        bool stop(TimerSize CmdTimeoutMs = 0);

        /**
         *  Reset a timer. This changes the state to active.
         *
         *  @param CmdTimeout How long to wait to send this command to the
         *         timer code.
         *  @returns true if this command will be sent to the timer code,
         *           false if it will not (i.e. timeout).
         */
        bool reset(TimerSize CmdTimeoutMs = 0);

        /**
         *  Change a timer's period.
         *
         *  @param NewPeriod The period in ms.
         *  @param CmdTimeout How long to wait to send this command to the
         *         timer code.
         *  @returns true if this command will be sent to the timer code,
         *           false if it will not (i.e. timeout).
         */
        bool setPeriod(TimerSize ms,
                       TimerSize CmdTimeoutMs = 0);

        //return period in ms
        uint16_t getPeriod();

    private:

        TimerImpl m_impl;
    };

}
#endif /* TIMER_HH_ */
