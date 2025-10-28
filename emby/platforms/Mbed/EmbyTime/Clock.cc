#include <EmbyTime/Clock.hh>
#include "EmbyDebug/assert.hh"

#include "mbed.h"
#include <EmbyTime/DateTime.hh>

namespace EmbyTime
{

    Clock::Clock()
        : m_impl()
    {
    }

    bool Clock::getTime(EmbyTime::DateTime& datetime) const
    {
        time_t seconds = time(NULL);
        if (seconds == 0)
        {
            return false;
        }
        auto tm = localtime(&seconds);
        auto y  = tm->tm_year + 1900; // 1990 is based year
        auto m  = tm->tm_mon + 1;     // month is based 0
        auto d  = tm->tm_mday;
        Date date((EmbyTime::DayOfMonth)d,
                  (EmbyTime::MonthOfYear)m,
                  (EmbyTime::Year)y,
                  Date::computeDayOfWeek(y, m, d));
        Time time(tm->tm_hour, tm->tm_min, tm->tm_sec);
        datetime = DateTime(date, time);
        return true;
    }

    bool Clock::setTime(DateTime& dateTime) const
    {
        TimeStamp secs;
        if (DateTime::getTimeStamp(secs, dateTime))
        {
            set_time(secs);
            return true;
        }
        return false;
    }

} // namespace EmbyTime
