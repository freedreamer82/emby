
#ifndef TIMERIMPL_HH_FREERTOS
#define TIMERIMPL_HH_FREERTOS
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
