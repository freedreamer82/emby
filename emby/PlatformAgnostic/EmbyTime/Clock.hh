/**
 * @addtogroup Time
 *
 *
 * @file EmbyTime/Clock.hh
 * @author Marco Garzola
 * @version 1.0
 * @date 11/11/2014
 *
 * @notes
 * @history
 *
 */

#if !defined( EMBYTIME_LOG_CLOCK_HH )
#define EMBYTIME_LOG_CLOCK_HH

#include <EmbyLibs/Singleton.hh>

#include <EmbyTime/Clock_Impl.hh>
#include <EmbyTime/DateTime.hh>
#include <EmbyTime/Date.hh>
#include <EmbyTime/Time.hh>


namespace EmbyTime
{
	/*
	 * Empty Clock.
	 * */
	class Clock : public EmbyLibs::Singleton<Clock>
	{
		public:

			Clock();
			/*
			 * Set System Clock from DateTime
			 * @param datetime time to be set
			 * */
			bool
			setTime( DateTime& datetime) const;

			/*
			 * Get  System Clock
			 * @retval datetime value
			 * */
			bool
			getTime(DateTime& dt) const;

			//commodity function to set time
			static bool setTime(TimeStamp & ts)  
			{
				EmbyTime::DateTime  dateTime;
				EmbyTime::TimeStamp s = ts;
				EmbyTime::DateTime::getDateTime(dateTime, s);
				return EmbyTime::Clock::get().setTime(dateTime);
			}

		private:
			ClockImpl m_impl;
	};

}
#endif
