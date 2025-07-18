//
// Created by mark on 09/11/23.
//

#ifndef PUPS_STM32XX_LIB_H
#define PUPS_STM32XX_LIB_H


#if defined(STM32F7xx)

#include <stdint.h>
#include "stm32f7xx_hal.h"
#include "stm32f7xx_ll_usart.h"
#include "stm32f7xx.h"

#elif defined(STM32L4xx)

#include <stdint.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx.h"

#elif defined(STM32H5xx)

#include <stdint.h>
#include "stm32h5xx_hal.h"
#include "stm32h5xx_hal_rtc.h"
#include "stm32h5xx_ll_usart.h"
#include "stm32h5xx.h"

#elif defined(STM32H7xx)

#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_rtc.h"
#include "stm32h7xx_ll_usart.h"
#include "stm32h7xx.h"

#else
#error "compiler error, select your STM family"
#endif
#endif //PUPS_STM32XX_LIB_H
