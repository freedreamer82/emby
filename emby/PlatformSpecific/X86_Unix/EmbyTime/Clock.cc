#include <EmbyTime/Clock.hh>
#include "EmbyDebug/assert.hh"
#include <ctime>
#include <sys/time.h>

namespace EmbyTime
{
    static constexpr uint16_t BASE_YEAR_LINUX = 1900;

    Clock::Clock() : m_impl()
    {
        // Nothing to initialize on Linux
    }

    Clock::~Clock() {
        // Nothing to clean up
    }

    bool Clock::getTime(EmbyTime::DateTime &datetime) const
    {
        std::time_t now = std::time(nullptr);
        if (now == -1)
            return false;

        std::tm tmStruct = {};
        if (!gmtime_r(&now, &tmStruct))
            return false;

        auto y = tmStruct.tm_year + BASE_YEAR_LINUX;
        auto m = tmStruct.tm_mon + 1; // tm_mon is 0-based
        auto d = tmStruct.tm_mday;

        Date date((EmbyTime::DayOfMonth)d,
                  (EmbyTime::MonthOfYear)m,
                  (EmbyTime::Year)y,
                  Date::computeDayOfWeek(y, m, d));
        Time time(tmStruct.tm_hour, tmStruct.tm_min, tmStruct.tm_sec);
        datetime = DateTime(date, time);

        return true;
    }

    bool Clock::setTime(DateTime &dateTime) const
    {

        // Validate input ranges
        if (static_cast<uint8_t>(dateTime.getDate().getMonth()) < 1 || static_cast<uint8_t>(dateTime.getDate().getMonth()) > 12 ||
            dateTime.getDate().getDay() < 1 || dateTime.getDate().getDay() > 31 ||
            dateTime.getTime().getHours() > 23 || dateTime.getTime().getMinutes() > 59 ||
            dateTime.getTime().getSeconds() > 59) {
            return false;
        }

        std::tm tmStruct = {};
        tmStruct.tm_year = dateTime.getDate().getYear() - BASE_YEAR_LINUX;
        tmStruct.tm_mon = static_cast<uint8_t>(dateTime.getDate().getMonth() - 1); // tm_mon is 0-based
        tmStruct.tm_mday = dateTime.getDate().getDay();
        tmStruct.tm_hour = dateTime.getTime().getHours();
        tmStruct.tm_min = dateTime.getTime().getMinutes();
        tmStruct.tm_sec = dateTime.getTime().getSeconds();
        tmStruct.tm_isdst = -1;

        std::time_t newTime = std::mktime(&tmStruct);
        if (newTime == -1)
            return false;

        timeval tv;
        tv.tv_sec = newTime;
        tv.tv_usec = 0;

        // Setting system time should require root privileges
        if (settimeofday(&tv, nullptr) != 0)
            return false;

        return true;
    }

    bool Clock::isRTCRunning() const
    {
        // On Linux, assume system clock is always running
        return true;
    }
} // namespace EmbyTime
