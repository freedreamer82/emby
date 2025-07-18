//
// Created by marco on 18/01/22.
//

#if !defined(EMBYTIME_CLOCK_IMPL_HH)
#define EMBYTIME_CLOCK_IMPL_HH

extern "C"
{
#include "stm32xx_lib.h"
}

namespace EmbyTime
{
    struct ClockImpl
    {
        RTC_HandleTypeDef rtc;
    };
}// namespace EmbyTime

#endif // EMBYTIME_CLOCK_IMPL_HH



