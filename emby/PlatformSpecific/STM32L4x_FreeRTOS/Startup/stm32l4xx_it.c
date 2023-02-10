/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
 #include "stm32l4xx_it.h"
#include "stm32l4xx.h"
#include "FreeRTOS.h"
#include "task.h"


extern void xPortSysTickHandler( void );
/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */

void  hard_fault_handler_c(unsigned int * hardfault_args)
{

#ifdef BUILD_DEBUG
    volatile unsigned int stacked_r0;
    volatile unsigned int stacked_r1;
    volatile unsigned int stacked_r2;
    volatile unsigned int stacked_r3;
    volatile unsigned int stacked_r12;
    volatile unsigned int stacked_lr;
    volatile unsigned int stacked_pc;
    volatile unsigned int stacked_psr;

    stacked_r0 = ((unsigned long) hardfault_args[0]);
    stacked_r1 = ((unsigned long) hardfault_args[1]);
    stacked_r2 = ((unsigned long) hardfault_args[2]);
    stacked_r3 = ((unsigned long) hardfault_args[3]);

    stacked_r12 = ((unsigned long) hardfault_args[4]);
    stacked_lr = ((unsigned long) hardfault_args[5]);
    stacked_pc = ((unsigned long) hardfault_args[6]);
    stacked_psr = ((unsigned long) hardfault_args[7]);

    for(;;);

#else
    NVIC_SystemReset();
#endif
}

void HardFault_Handler(void)
{
#ifdef BUILD_DEBUG
    asm volatile(
    "movs r0, #4\t\n"
    "mov  r1, lr\t\n"
    "tst  r0, r1\t\n" /* Check EXC_RETURN[2] */
    "beq 1f\t\n"
    "mrs r0, psp\t\n"
    "ldr r1,=hard_fault_handler_c\t\n"
    "bx r1\t\n"
    "1:mrs r0,msp\t\n"
    "ldr r1,=hard_fault_handler_c\t\n"
    : /* no output */
    : /* no input */
    : "r0" /* clobber */
    );

#else
    NVIC_SystemReset();
#endif
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
#ifdef BUILD_DEBUG
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
#else
    NVIC_SystemReset();
#endif
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
#ifdef BUILD_DEBUG
    /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
#else
    NVIC_SystemReset();
#endif
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
#ifdef BUILD_DEBUG
    /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
#else
    NVIC_SystemReset();
#endif
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
//void SysTick_Handler(void)
//{
//  /* USER CODE BEGIN SysTick_IRQn 0 */
//
//  /* USER CODE END SysTick_IRQn 0 */
//  HAL_IncTick();
//#if (INCLUDE_xTaskGetSchedulerState == 1 )
//  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
//  {
//#endif /* INCLUDE_xTaskGetSchedulerState */
//  xPortSysTickHandler();
//#if (INCLUDE_xTaskGetSchedulerState == 1 )
//  }
//#endif /* INCLUDE_xTaskGetSchedulerState */
//  /* USER CODE BEGIN SysTick_IRQn 1 */
//
//  /* USER CODE END SysTick_IRQn 1 */
//}


void SysTick_Handler(void) {
    HAL_IncTick();
#ifndef USE_BOOTLOADER
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }

    ///@todo remove me from here...see issues #12
    //TSC_TIMER_Callback();

#endif
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
