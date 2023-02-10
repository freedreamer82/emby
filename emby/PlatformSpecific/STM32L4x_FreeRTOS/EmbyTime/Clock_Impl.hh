//
// Created by marco on 18/01/22.
//

#ifndef DVC_WHEEL_SCALE_CLOCK_IMPL_HH
#define DVC_WHEEL_SCALE_CLOCK_IMPL_HH

#include "stm32l4xx.h"

namespace EmbyTime
{

    struct ClockImpl
    {
        RTC_HandleTypeDef rtc;
    };
}




#endif //DVC_WHEEL_SCALE_CLOCK_IMPL_HH
