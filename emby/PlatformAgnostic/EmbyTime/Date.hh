/**
 * @addtogroup Time
 *
 *
 * @file EmbyTime/Date.hh
 * @author Marco Garzola
 * @version 1.0
 * @date 18/05/2007
 *
 * @notes
 * @history
 *
 */

#if !defined( EMBY_DATE_HH )
#define EMBY_DATE_HH

#include <EmbyTime/TimeStamp.hh>
#include <EmbyLibs/String.hh>

namespace EmbyTime
{

	typedef uint8_t   DayOfMonth;
	typedef uint16_t  Year;

	enum class DayOfWeek:uint8_t
	{
	        Sunday = 0,
	        Monday ,
	        Tuesday,
	        Wednesday,
	        Thursday,
	        Friday,
	        Saturday,
	        Unkwown
	};


	enum class MonthOfYear:uint8_t
	{
	    MonthOfYear_Jan = 1,
	    MonthOfYear_Feb,
	    MonthOfYear_Mar,
	    MonthOfYear_Apr,
	    MonthOfYear_May,
	    MonthOfYear_Jun,
	    MonthOfYear_Jul,
	    MonthOfYear_Aug,
	    MonthOfYear_Sep,
	    MonthOfYear_Oct,
	    MonthOfYear_Nov,
	    MonthOfYear_Dec
	};

	class Date
	{
		public:

			Date( EmbyTime::DayOfMonth     day ,
				  EmbyTime:: MonthOfYear   month ,
				  EmbyTime::Year year ,EmbyTime::DayOfWeek dow =  EmbyTime::DayOfWeek::Unkwown ) :
				          m_day(day) , m_month(month) ,
				          m_year(year),m_dayOfweek{dow}
			{ }

			DayOfMonth
			getDay() const
			{
				return m_day;
			}

			DayOfWeek
                        getDayOfWeek() const
                        {
                                return m_dayOfweek;
                        }

			MonthOfYear
			getMonth() const
			{
				return m_month;
			}

			Year
			getYear() const
			{
				return m_year;
			}

			static EmbyTime::DayOfWeek computeDayOfWeek(uint16_t y, uint8_t m, uint8_t d);

                        static EmbyLibs::String dayOfWeekToString(EmbyTime::DayOfWeek day);


		private:
			DayOfMonth      m_day;
			MonthOfYear     m_month;
			Year            m_year;
			DayOfWeek       m_dayOfweek;
	};

}
#endif
