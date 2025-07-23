//
// Created by marco on 18/01/22.
//
#ifndef EMBY_CLOCK_IMPL_HH
#define EMBY_CLOCK_IMPL_HH

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

#endif // EMBY_CLOCK_IMPL_HH



