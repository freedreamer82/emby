
#include <EmbyTime/Date.hh>
#include <EmbyLibs/String.hh>

namespace EmbyTime
{


        //[in]    y       Year
        //[in]    m       Month of year (in range 1 to 12)
        //[in]    d       Day of month (in range 1 to 31)
        EmbyTime::DayOfWeek  Date::computeDayOfWeek(uint16_t y, uint8_t m, uint8_t d)
        {
            uint8_t h;
            uint8_t j;
            uint8_t k;

            // January and February are counted as months 13 and 14 of the previous year
            if (m <= 2)
            {
                m += 12;
                y -= 1;
            }

            // J is the century
            j = y / 100;
            // K the year of the century
            k = y % 100;

            // Compute H using Zeller's congruence
            h = d + (26 * (m + 1) / 10) + k + (k / 4) + (5 * j) + (j / 4);

            // day in range 1-7 monday-sunday
            auto day = ((h + 5) % 7) + 1;

            if (day == 7)
            {
                day = 0; // we start from sunday
            }
            else if (day >= 1 && day <= 6)
            {
                // do nothing right range!!
            }
            else
            {
                day = (int)EmbyTime::DayOfWeek::Unkwown; // unkwown value
            }
            // Return the day converted
            return (EmbyTime::DayOfWeek) day;
        }

        EmbyLibs::String Date::dayOfWeekToString(EmbyTime::DayOfWeek day)
        {
            switch (day)
            {
                case EmbyTime::DayOfWeek::Sunday:
                    return EmbyLibs::String("sun");
                    break;
                case EmbyTime::DayOfWeek::Monday:
                    return EmbyLibs::String("mon");
                    break;
                case EmbyTime::DayOfWeek::Tuesday:
                    return EmbyLibs::String("tue");
                    break;
                case EmbyTime::DayOfWeek::Wednesday:
                    return EmbyLibs::String("wed");
                    break;
                case EmbyTime::DayOfWeek::Thursday:
                    return EmbyLibs::String("thu");
                    break;
                case EmbyTime::DayOfWeek::Friday:
                    return EmbyLibs::String("fri");
                    break;
                case EmbyTime::DayOfWeek::Saturday:
                    return EmbyLibs::String("sat");
                    break;
            }
            return EmbyLibs::String("unkwown");
        }



}
