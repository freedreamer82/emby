#include <EmbyMachine/EmbyMachine.hh>
#include <cstdlib>
#include <cstring>
#include "stm32xx_lib.h"

namespace EmbyMachine
{
    static DCACHE_HandleTypeDef g_hdcache1;
    // Necessary to prevent the linker from removing the config section
    static __attribute__ ((section(".config"), used)) uint32_t configArea = 0xEEEEEEEE;

    extern "C"
    {
        void assert_failed(uint8_t *file, uint32_t line)
        {
            // Custom implementation to report file and line
            // printf("Wrong parameters value: file %s on line %d\r\n", file, line);

            // Infinite loop
            abort();
        }

        /**
         * @brief UART MSP Initialization
         * Configures hardware resources for UART
         */
        void HAL_UART_MspInit(UART_HandleTypeDef* huart)
        {
            GPIO_InitTypeDef GPIO_InitStruct = {0};
            RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

            if (huart->Instance == USART3)
            {
                // Peripheral clock configuration
                PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
                PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
                if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
                {
                    EmbyDebug_ASSERT_FAIL();
                }

                __HAL_RCC_USART3_CLK_ENABLE();
                __HAL_RCC_GPIOD_CLK_ENABLE();

                // GPIO configuration for USART3
                GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
                HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
            }

            if (huart->Instance == UART4)
            {
                // Peripheral clock configuration
                PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4;
                PeriphClkInitStruct.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
                if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
                {
                    EmbyDebug_ASSERT_FAIL();
                }

                __HAL_RCC_UART4_CLK_ENABLE();
                __HAL_RCC_GPIOD_CLK_ENABLE();

                // GPIO configuration for UART4
                GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_11;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
                HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
            }
        }

        /**
         * @brief UART MSP De-Initialization
         * Disables hardware resources for UART
         */
        void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
        {
            if (huart->Instance == USART3)
            {
                __HAL_RCC_USART3_CLK_DISABLE();
                HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8 | GPIO_PIN_9);
            }
        }

        /**
         * @brief System Clock Configuration
         */
        void SystemClock_Config(void)
        {
            RCC_OscInitTypeDef RCC_OscInitStruct = {0};
            RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

            // Configure the voltage regulator
            __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
            while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

            // Configure RCC oscillators
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

            // Configure CPU, AHB and APB buses
            RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                        | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                                        | RCC_CLOCKTYPE_PCLK3;
            RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
            RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
            RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
            RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
            RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

            if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
            {
                EmbyDebug_ASSERT_FAIL();
            }

            // Configure programming delay
            __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
        }

        // Code for EVAL BOARD commented out
        /*
        void SystemClock_Config(void)
        {
            // ...
        }
        */
    }

    // Override of the WEAK function from the SDK
    void init()
    {
        __HAL_RCC_CRC_CLK_ENABLE();
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
