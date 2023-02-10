#include <EmbyTime/DateTime.hh>
#include <EmbyDebug/assert.hh>
#include <utility>

namespace
{
    static uint32_t constexpr
    SECONDS_IN_A_DAY = 86400U;
    static uint32_t constexpr
    SECONDS_IN_A_HOUR = 3600U;
    static uint32_t constexpr
    SECONDS_IN_A_MINUTE = 60U;
    static uint32_t constexpr
    DAYS_IN_A_YEAR = 365U;
}

namespace EmbyTime
{


    bool
    DateTime::getTimeStamp(TimeStamp &ts, DateTime &dateTime)
    {
        /* Number of days from begin of the non Leap-year*/
        /* Number of days from begin of the non Leap-year*/
        uint16_t
        constexpr monthDays[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};
        uint32_t seconds;

        const uint8_t month = static_cast<uint8_t>(dateTime.getDate().getMonth());

        /* Compute number of days from 1970 till given year*/
        seconds = (dateTime.getDate().getYear() - 1970U) * DAYS_IN_A_YEAR;
        /* Add leap year days */
        seconds += ((dateTime.getDate().getYear() / 4) - (1970U / 4));
        /* Add number of days till given month*/
        seconds += monthDays[int(month)];
        /* Add days in given month. We subtract the current day as it is
         * represented in the hours, minutes and seconds field*/
        seconds += (dateTime.getDate().getDay() - 1);
        /* For leap year if month less than or equal to Febraury, decrement day counter*/
        if ((!(dateTime.getDate().getYear() & 3U)) && (month <= 2U))
        {
            seconds--;
        }

        seconds = (seconds * SECONDS_IN_A_DAY) + (dateTime.getTime().getHours() * SECONDS_IN_A_HOUR) +
                  (dateTime.getTime().getMinutes() * SECONDS_IN_A_MINUTE) + dateTime.getTime().getSeconds();

        ts = std::move(seconds);
        return true;

    }


    //from secs to datetime
    bool
    DateTime::getDateTime(DateTime &dt, TimeStamp &ts, uint16_t startYear)
    {
        uint32_t x;
        uint32_t secondsRemaining, days;
        uint16_t daysInYear;
        uint32_t hours;
        uint32_t mins;
        uint32_t secs;
        uint32_t year;
        uint32_t month;

        /* Table of days in a month for a non leap year. First entry in the table is not used,
         * valid months start from 1
         */
        uint8_t daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

        /* Start with the seconds value that is passed in to be converted to date time format */
        secondsRemaining = ts;

        /* Calcuate the number of days, we add 1 for the current day which is represented in the
         * hours and seconds field
         */
        days = secondsRemaining / SECONDS_IN_A_DAY + 1;

        /* Update seconds left*/
        secondsRemaining = secondsRemaining % SECONDS_IN_A_DAY;

        /* Calculate the datetime hour, minute and second fields */
        hours = secondsRemaining / SECONDS_IN_A_HOUR;
        secondsRemaining = secondsRemaining % SECONDS_IN_A_HOUR;
        mins = secondsRemaining / 60U;
        secs = secondsRemaining % SECONDS_IN_A_MINUTE;

        /* Calculate year */
        daysInYear = DAYS_IN_A_YEAR;
        year = startYear;
        while (days > daysInYear)
        {
            /* Decrease day count by a year and increment year by 1 */
            days -= daysInYear;
            year++;

            /* Adjust the number of days for a leap year */
            if (year & 3U)
            {
                daysInYear = DAYS_IN_A_YEAR;
            }
            else
            {
                daysInYear = DAYS_IN_A_YEAR + 1;
            }
        }

        /* Adjust the days in February for a leap year */
        if (!(year & 3U))
        {
            daysPerMonth[2] = 29U;
        }

        for (x = 1U; x <= 12U; x++)
        {
            if (days <= daysPerMonth[x])
            {
                month = x;
                break;
            }
            else
            {
                days -= daysPerMonth[x];
            }
        }


        auto dow = Date::computeDayOfWeek(year, month, days);
//	           dow= dow - 1; // range is 1-7 from the function but enum start from 0

        auto date = EmbyTime::Date((EmbyTime::DayOfMonth) (days),
                                   (EmbyTime::MonthOfYear) (month),  //month
                                   (EmbyTime::Year) (year), //year,
                                   (EmbyTime::DayOfWeek) dow    //day
        );

        auto time = EmbyTime::Time(hours, mins, secs);//sec

        EmbyTime::DateTime datetime(date, time);

        dt = std::move(datetime);
        return true;
    }

    EmbyLibs::String DateTime::dateTimeToString(DateTime &dateTime)
    {
        // Wed Jan 26 14:04:54 2022
        EmbyLibs::String out;
        EmbyLibs::sprintf(out,
                          "%d/%d/%d %d:%d:%d",
                          dateTime.getDate().getDay(),
                          dateTime.getDate().getMonth(),
                          dateTime.getDate().getYear(),
                          dateTime.getTime().getHours(),
                          dateTime.getTime().getMinutes(),
                          dateTime.getTime().getSeconds());
        return out;
    }
}
