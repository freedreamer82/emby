#include <EmbyMachine/EmbyMachine.hh>
#include <cstdlib>

#include <cstring>
#include "stm32xx_lib.h"


namespace EmbyMachine
{

  static DCACHE_HandleTypeDef g_hdcache1;
  ///@todo this is necessary otherwise the link remove the config section
  static __attribute__ ((section(".config"), used)) uint32_t configArea = 0xEEEEEEEE;



extern "C"
    {

    void assert_failed(uint8_t *file, uint32_t line) {
        /* User can add his own implementation to report the file name and line number,
         ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

        /* Infinite loop */
        abort();
    }

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
    void HAL_UART_MspInit(UART_HandleTypeDef* huart)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
        if(huart->Instance==USART3)
        {
            /* USER CODE BEGIN USART3_MspInit 0 */

            /* USER CODE END USART3_MspInit 0 */

            /** Initializes the peripherals clock
            */
            PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
            PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
            if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
            {
                EmbyDebug_ASSERT_FAIL();
            }

            /* Peripheral clock enable */
            __HAL_RCC_USART3_CLK_ENABLE();

            __HAL_RCC_GPIOD_CLK_ENABLE();
            /**USART3 GPIO Configuration
            PD8     ------> USART3_TX
            PD9     ------> USART3_RX
            */
            GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
            HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

            /* USER CODE BEGIN USART3_MspInit 1 */

            /* USER CODE END USART3_MspInit 1 */
        }
        if(huart->Instance==UART4)
        {
          /** Initializes the peripherals clock
           */
          PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4;
          PeriphClkInitStruct.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
          if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
          {
            EmbyDebug_ASSERT_FAIL();
          }

          /* Peripheral clock enable */
          __HAL_RCC_UART4_CLK_ENABLE();

          __HAL_RCC_GPIOD_CLK_ENABLE();
          /**USART3 GPIO Configuration
          PD12     ------> USART3_TX
          PD11     ------> USART3_RX
          */
          GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_11;
          GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
          GPIO_InitStruct.Pull = GPIO_NOPULL;
          GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
          GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
          HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

          /* USER CODE BEGIN USART3_MspInit 1 */

          /* USER CODE END USART3_MspInit 1 */
        }

    }

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
    void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
    {
        if(huart->Instance==USART3)
        {
            /* USER CODE BEGIN USART3_MspDeInit 0 */

            /* USER CODE END USART3_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_USART3_CLK_DISABLE();

            /**USART3 GPIO Configuration
            PD8     ------> USART3_TX
            PD9     ------> USART3_RX
            */
            HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8|GPIO_PIN_9);

            /* USER CODE BEGIN USART3_MspDeInit 1 */

            /* USER CODE END USART3_MspDeInit 1 */
        }

    }

    /**
      * @brief RTC MSP Initialization
      * This function configures the hardware resources used in this example
      * @param hrtc: RTC handle pointer
      * @retval None
      */
    void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
    {
        RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
        if(hrtc->Instance==RTC)
        {
            /* USER CODE BEGIN RTC_MspInit 0 */

            /* USER CODE END RTC_MspInit 0 */

            /** Initializes the peripherals clock
            */
            PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
            PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
            if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
            {
                EmbyDebug_ASSERT_FAIL();
            }

            /* Peripheral clock enable */
            __HAL_RCC_RTC_ENABLE();
            __HAL_RCC_RTCAPB_CLK_ENABLE();
            /* USER CODE BEGIN RTC_MspInit 1 */

            /* USER CODE END RTC_MspInit 1 */

        }

    }

    /**
      * @brief RTC MSP De-Initialization
      * This function freeze the hardware resources used in this example
      * @param hrtc: RTC handle pointer
      * @retval None
      */
    void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
    {
        if(hrtc->Instance==RTC)
        {
            /* USER CODE BEGIN RTC_MspDeInit 0 */

            /* USER CODE END RTC_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_RTC_DISABLE();
            __HAL_RCC_RTCAPB_CLK_DISABLE();
            /* USER CODE BEGIN RTC_MspDeInit 1 */

            /* USER CODE END RTC_MspDeInit 1 */
        }

    }

    /**
  * @brief System Clock Configuration
  * @retval None
     */
    void SystemClock_Config(void)
    {
      RCC_OscInitTypeDef RCC_OscInitStruct = {0};
      RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

      /** Configure the main internal regulator output voltage
       */
      __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

      while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

      /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
       */
      RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_CSI;
      RCC_OscInitStruct.CSIState = RCC_CSI_ON;
      RCC_OscInitStruct.CSICalibrationValue = RCC_CSICALIBRATION_DEFAULT;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_CSI;
      RCC_OscInitStruct.PLL.PLLM = 1;
      RCC_OscInitStruct.PLL.PLLN = 125;
      RCC_OscInitStruct.PLL.PLLP = 2;
      RCC_OscInitStruct.PLL.PLLQ = 2;
      RCC_OscInitStruct.PLL.PLLR = 2;
      RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
      RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
      RCC_OscInitStruct.PLL.PLLFRACN = 0;
      if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
      {
        EmbyDebug_ASSERT_FAIL();
      }

      /** Initializes the CPU, AHB and APB buses clocks
       */
      RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                    |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                    |RCC_CLOCKTYPE_PCLK3;
      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
      RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
      RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

      if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
      {
        EmbyDebug_ASSERT_FAIL();
      }

      /** Configure the programming delay
       */
      __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
    }


     //////EVAL BOARD
//    void SystemClock_Config(void)
//    {
//      RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//      RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//
//      /** Configure the main internal regulator output voltage
//       */
//      __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
//
//      while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
//
//      /** Initializes the RCC Oscillators according to the specified parameters
//        * in the RCC_OscInitTypeDef structure.
//       */
//      RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
//      RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
//      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//      RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
//      RCC_OscInitStruct.PLL.PLLM = 4;
//      RCC_OscInitStruct.PLL.PLLN = 250;
//      RCC_OscInitStruct.PLL.PLLP = 2;
//      RCC_OscInitStruct.PLL.PLLQ = 2;
//      RCC_OscInitStruct.PLL.PLLR = 2;
//      RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_1;
//      RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
//      RCC_OscInitStruct.PLL.PLLFRACN = 0;
//      if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//      {
//        EmbyDebug_ASSERT_FAIL();
//      }
//
//      /** Initializes the CPU, AHB and APB buses clocks
//       */
//      RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                                    |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
//                                    |RCC_CLOCKTYPE_PCLK3;
//      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
//      RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
//      RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;
//
//      if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
//      {
//        EmbyDebug_ASSERT_FAIL();
//      }
//    }


    }
//override WEAK function of the SDK
    void init()
    {
        //__HAL_RCC_PWR_CLK_ENABLE();
        /* The STM32 CRC IP clock should be enabled to use the network runtime library */
        __HAL_RCC_CRC_CLK_ENABLE();
        //__HAL_RCC_SYSCFG_CLK_ENABLE();
        HAL_Init();
#ifndef EMBY_BOOTLOADER
        HAL_ICACHE_Enable();
        g_hdcache1.Instance = DCACHE1;
        g_hdcache1.Init.ReadBurstType = DCACHE_READ_BURST_WRAP;
        HAL_DCACHE_Init(&g_hdcache1);
#endif
        SystemClock_Config();

    }

    void abort()
    {
#ifdef EMBY_BUILD_DEBUG
        while (1) {};
#else
      EmbySystem::reboot();
#endif

    }


}

namespace EmbySystem
{
    void cacheDEnable()
    {
    }
    void cacheIEnable()
    {
    }
    void cacheDCleanAndInvalidate()
    {
    }
}