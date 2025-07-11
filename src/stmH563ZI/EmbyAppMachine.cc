#include <EmbyMachine/EmbyMachine.hh>
#include <cstdlib>
#include <cstring>
#include "stm32xx_lib.h"

namespace EmbyMachine
{
    static DCACHE_HandleTypeDef g_hdcache1;
    // This is necessary otherwise the linker removes the config section
    static __attribute__((section(".config"), used)) uint32_t configArea = 0xEEEEEEEE;

    extern "C"
    {
        void assert_failed(uint8_t *file, uint32_t line)
        {
            // User can add implementation to report the file name and line number
            // Example: printf("Wrong parameters value: file %s on line %d\r\n", file, line)
            // Infinite loop
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
            if (huart->Instance == USART3)
            {
                // Initializes the peripherals clock
                PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
                PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
                if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
                {
                    Error_Handler();
                }

                // Peripheral clock enable
                __HAL_RCC_USART3_CLK_ENABLE();
                __HAL_RCC_GPIOD_CLK_ENABLE();

                // USART3 GPIO Configuration
                // PD8 ------> USART3_TX
                // PD9 ------> USART3_RX
                GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
                HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
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
            if (huart->Instance == USART3)
            {
                /* Peripheral clock disable */
                __HAL_RCC_USART3_CLK_DISABLE();

                /**USART3 GPIO Configuration
                PD8     ------> USART3_TX
                PD9     ------> USART3_RX
                */
                HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8 | GPIO_PIN_9);
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

            // Configure the main internal regulator output voltage
            __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
            while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

            // Initializes the RCC Oscillators according to the specified parameters
            RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_CSI;
            RCC_OscInitStruct.CSIState = RCC_CSI_ON;
            RCC_OscInitStruct.CSICalibrationValue = RCC_CSICALIBRATION_DEFAULT;
            RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
            RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_CSI;
            RCC_OscInitStruct.PLL.PLLM = 4;
            RCC_OscInitStruct.PLL.PLLN = 250;
            RCC_OscInitStruct.PLL.PLLP = 2;
            RCC_OscInitStruct.PLL.PLLQ = 2;
            RCC_OscInitStruct.PLL.PLLR = 2;
            RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_0;
            RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
            RCC_OscInitStruct.PLL.PLLFRACN = 0;
            if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
            {
                Error_Handler();
            }

            // Initializes the CPU, AHB and APB buses clocks
            RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                        | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                                        | RCC_CLOCKTYPE_PCLK3;
            RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
            RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
            RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
            RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
            RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

            if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
            {
                Error_Handler();
            }

            // Configure the programming delay
            __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
        }

        // Override WEAK function of the SDK
        void init()
        {
            // __HAL_RCC_PWR_CLK_ENABLE();
            // The STM32 CRC IP clock should be enabled to use the network runtime library
            __HAL_RCC_CRC_CLK_ENABLE();
            // __HAL_RCC_SYSCFG_CLK_ENABLE();
            HAL_Init();
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
