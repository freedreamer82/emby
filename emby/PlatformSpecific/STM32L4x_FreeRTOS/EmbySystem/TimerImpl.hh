/*
 * TimerImpl.hh
 *
 *  Created on: 28/feb/2017
 *      Author: mgarzola
 */

#ifndef TIMERIMPL_HH_
#define TIMERIMPL_HH_
#include "FreeRTOS.h"
#include "timers.h"

namespace EmbySystem
{
    struct TimerImpl
    {
        TimerHandle_t  m_handle;
        bool isRunning;
    };
}




#endif /* TIMERIMPL_HH_ */
