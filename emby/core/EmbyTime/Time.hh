/**
 * @addtogroup Time
 *
 *
 * @file EmbyTime/Time.hh
 * @author Marco Garzola
 * @version 1.0
 * @date 18/05/2007
 *
 * @notes
 * @history
 *
 */

#if !defined( EMBYLOG_TIME_HH )
#define EMBYLOG_TIME_HH

namespace EmbyTime
{

	typedef uint8_t Hour;
	typedef uint8_t Min;
	typedef uint8_t Sec;

	class Time
	{
		public:

			Time( Hour hour , Min min ,Sec secs , uint32_t millis = 0 ) :
				m_hour(hour) ,m_min(min) , m_secs(secs) , m_millis(millis)
			{}

			/**
			 * Default constructor with default values
			 * */
			Time() :m_hour(0) ,m_min(0) , m_secs(0) , m_millis(0)
			{ }

			Hour
			getHours() const
			{
				return m_hour;
			}

			uint32_t
            getMillis() const
            {
                    return m_millis;
            }

			Min
			getMinutes() const
			{
				return m_min;
			}

			Sec
			getSeconds() const
			{
				return m_secs;
			}

            bool
            isValid() const
            {
                return (m_hour < 24) && (m_min < 60) && (m_secs < 60);
            }

		private:
			Hour  	 m_hour;
			Min		 m_min;
			Sec		 m_secs;
			uint32_t m_millis;
	};

}
#endif
