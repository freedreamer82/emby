/**
 * @addtogroup Time
 *
 *
 * @file EmbyTime/DateTime.hh
 * @author Marco Garzola
 * @version 1.0
 * @date 18/05/2007
 *
 * @notes
 * @history
 *
 */

#if !defined( EMBYLOG_DATE_TIME_HH )
#define EMBYLOG_DATE_TIME_HH

#include <EmbyTime/TimeStamp.hh>
#include <EmbyTime/Date.hh>
#include <EmbyTime/Time.hh>
#include <EmbyLibs/StringUtils.hh>

namespace EmbyTime
{
    /**
     * Unix start time : 1 jan 1970
     * */
#define TIME_DEFAULT_DAY                                 1
#define TIME_DEFAULT_MONTH      MonthOfYear::MonthOfYear_Jan
#define TIME_DEFAULT_YEAR                              1970

    class DateTime
    {
    public:

        DateTime(Date const &date, Time const &time) :
                m_date(date), m_time(time)
        {}


        DateTime() :
                m_date(Date(TIME_DEFAULT_DAY, TIME_DEFAULT_MONTH, TIME_DEFAULT_YEAR)),
                m_time(Time())
        {}


        /*
         * Class Method to get TimeStamp from a DateTime
         * */
        static bool
        getTimeStamp(TimeStamp &ts, DateTime &dateTime);

        /*
        * Class Method to get TimeStamp from a DateTime
        * */
        static bool
        getDateTime(DateTime &dt, TimeStamp &ts, uint16_t startYear = TIME_DEFAULT_YEAR);


        /**
         * Instance method:
         * @return TimeStamp from the object DateTime
         * */
        bool
        getTimeStamp(TimeStamp &ts)
        {
            return getTimeStamp(ts, *this);
        }

        /**
         *
         */
        Date &
        getDate()
        {
            return m_date;
        }

        Time &
        getTime()
        {
            return m_time;
        }

        static EmbyLibs::String dateTimeToString(DateTime &dateTime);


    private:
        Date m_date;
        Time m_time;
    };

}
#endif
