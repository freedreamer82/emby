#include <EmbyTime/Clock.hh>
#include "EmbyDebug/assert.hh"

extern "C"
{
#include "stm32l4xx.h"
}


namespace EmbyTime
{

    static constexpr uint16_t BASE_YEAR_ST = 2000;

    Clock::Clock() : m_impl()
    {
        m_impl.rtc.Instance = RTC;

        /*##-1- Configure the RTC peripheral #######################################*/
        /* Configure RTC prescaler and RTC data registers */
        /* RTC configured as follow:
            - Hour Format    = Format 12
            - Asynch Prediv  = Value according to source clock
            - Synch Prediv   = Value according to source clock
            - OutPut         = Output Disable
            - OutPutPolarity = High Polarity
            - OutPutType     = Open Drain */
        m_impl.rtc.Init.HourFormat = RTC_HOURFORMAT_24;
        m_impl.rtc.Init.AsynchPrediv = 127;
        m_impl.rtc.Init.SynchPrediv = 255;
        m_impl.rtc.Init.OutPut = RTC_OUTPUT_DISABLE;
        m_impl.rtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
        m_impl.rtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
        m_impl.rtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
        if (HAL_RTC_Init(&m_impl.rtc) != HAL_OK)
        {
            EmbyDebug_ASSERT_FAIL();
        }
    }


    bool Clock::getTime(EmbyTime::DateTime &datetime) const
    {
        RTC_DateTypeDef sDate = {0};
        RTC_TimeTypeDef sTime = {0};

        if (HAL_RTC_GetDate((RTC_HandleTypeDef *) &m_impl.rtc, &sDate, RTC_FORMAT_BIN) == HAL_OK)
        {
            if (HAL_RTC_GetTime((RTC_HandleTypeDef *) &m_impl.rtc, &sTime, RTC_FORMAT_BIN) == HAL_OK)
            {
                auto y = sDate.Year + BASE_YEAR_ST; // 1990 is based year
                auto m = sDate.Month;     // month is based 0
                auto d = sDate.Date;
                Date date((EmbyTime::DayOfMonth) d,
                          (EmbyTime::MonthOfYear) m,
                          (EmbyTime::Year) y,
                          Date::computeDayOfWeek(y, m, d));
                Time time(sTime.Hours, sTime.Minutes, sTime.Seconds);
                datetime = DateTime(date, time);
                return true;
            }
        }
        return false;
    }

    bool Clock::setTime(DateTime &dateTime) const
    {
        RTC_TimeTypeDef sTime = {0};
        RTC_DateTypeDef sDate = {0};

        sTime.Hours = dateTime.getTime().getHours();
        sTime.Minutes = dateTime.getTime().getMinutes();
        sTime.Seconds = dateTime.getTime().getSeconds();
        sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sTime.StoreOperation = RTC_STOREOPERATION_SET;
        if (HAL_RTC_SetTime((RTC_HandleTypeDef *) &m_impl.rtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
        {
            return false;
        }
        sDate.WeekDay = int(dateTime.getDate().getDayOfWeek()) == 0 ? 7 : int(dateTime.getDate().getDayOfWeek());
        sDate.Month = int(dateTime.getDate().getMonth());
        sDate.Date = int(dateTime.getDate().getDay());
        sDate.Year = int(dateTime.getDate().getYear() - BASE_YEAR_ST);       // time.h is years since 1900, chip is years since 2000

        if (HAL_RTC_SetDate((RTC_HandleTypeDef *) &m_impl.rtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
        {
            return false;
        }

        return true;
    }

} // namespace EmbyTime
