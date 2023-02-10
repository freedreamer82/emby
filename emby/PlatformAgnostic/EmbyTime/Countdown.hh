/**
 * @addtogroup Time
 *
 *
 * @file EmbyTime/CountDown.hh
 * @author Marco Garzola
 * @version 1.0
 * @date 11/11/2014
 *
 * @notes
 * @history
 *
 */

#if !defined(EMBYTIME_COUNTDOWN_HH)
#define EMBYTIME_COUNTDOWN_HH

#include <EmbyTime/Millis.hh>
#include <System.hh>

namespace EmbyTime
{
    class Countdown
    {
    public:
        Countdown()
                : m_duration_ms(0), m_interval_end_ms(0)
        {
        }

        Countdown(uint64_t ms, bool start = false)
                : m_duration_ms(ms), m_interval_end_ms(0)
        {
            if (start)
            {
                countdownMs(m_duration_ms);
            }
        }

        bool isExpired()
        {
            return (m_interval_end_ms > 0) && (SailSystem::upTimeMs() >= m_interval_end_ms);
        }

        uint64_t getCountdownMs()
        {
            return m_duration_ms;
        }

        void reset()
        {
            countdownMs(m_duration_ms);
        }

        /*start the Final-countdown!! */
        bool countdownMs(uint64_t ms)
        {
            if (ms != 0)
            {
                m_duration_ms = ms;
            }
            if (m_duration_ms > 0)
            {
                m_interval_end_ms = SailSystem::upTimeMs() + m_duration_ms;
                return true;
            }
            else
            {
                return false;
            }
        }

        uint64_t leftMs()
        {
            return m_interval_end_ms > 0 ? m_interval_end_ms - SailSystem::upTimeMs() : 0;
        }

    private:
        uint64_t m_duration_ms;
        uint64_t m_interval_end_ms;
    };
} // namespace SailTime

#endif
