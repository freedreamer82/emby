#include <EmbyMachine/EmbyMachine.hh>
#include <cstdlib>

#include <cstring>
#include "stm32h7xx_hal.h"


namespace EmbyMachine
{

    extern "C"
    {

    UART_HandleTypeDef huart3;

    void assert_failed(uint8_t *file, uint32_t line)
    {
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
            PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
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
            /* USER CODE BEGIN RTC_MspDeInit 1 */

            /* USER CODE END RTC_MspDeInit 1 */
        }

    }

    void SystemClock_Config(uint32_t clockFreq)
    {
        //HAL_RCC_DeInit();

        switch (clockFreq){
            case 480: {
                RCC_OscInitTypeDef RCC_OscInitStruct = {0};
                RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

                /** Supply configuration update enable
                */
                //HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
                HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

                /** Configure the main internal regulator output voltage
                */
                __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

                while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

                /** Initializes the RCC Oscillators according to the specified parameters
                * in the RCC_OscInitTypeDef structure.
                */
                RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
                RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
                RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
                RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
                RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
                RCC_OscInitStruct.PLL.PLLM = 4;
                RCC_OscInitStruct.PLL.PLLN = 60;
                RCC_OscInitStruct.PLL.PLLP = 2;
                RCC_OscInitStruct.PLL.PLLQ = 2;
                RCC_OscInitStruct.PLL.PLLR = 2;
                RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
                RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
                RCC_OscInitStruct.PLL.PLLFRACN = 0;
                if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
                    EmbyDebug_ASSERT_FAIL();
                }

                /** Initializes the CPU, AHB and APB buses clocks
                */
                RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                                              | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
                RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
                RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
                RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
                RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
                RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
                RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
                RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

                if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
                    EmbyDebug_ASSERT_FAIL();
                }
                break;
            }
            case 350:
            {
                RCC_OscInitTypeDef RCC_OscInitStruct = {0};
                RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

                /** Supply configuration update enable
                */
                HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

                /** Configure the main internal regulator output voltage
                */
                __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

                while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

                /** Initializes the RCC Oscillators according to the specified parameters
                * in the RCC_OscInitTypeDef structure.
                */
                RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
                RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
                RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
                RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
                RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
                RCC_OscInitStruct.PLL.PLLM = 4;
                RCC_OscInitStruct.PLL.PLLN = 43;
                RCC_OscInitStruct.PLL.PLLP = 2;
                RCC_OscInitStruct.PLL.PLLQ = 4;
                RCC_OscInitStruct.PLL.PLLR = 2;
                RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
                RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
                RCC_OscInitStruct.PLL.PLLFRACN = 6144;
                if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
                {
                    EmbyDebug_ASSERT_FAIL();
                }

                /** Initializes the CPU, AHB and APB buses clocks
                */
                RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
                RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
                RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
                RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
                RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
                RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
                RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
                RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

                if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
                {
                    EmbyDebug_ASSERT_FAIL();
                }
                break;
            }
            case 300:
            {
                RCC_OscInitTypeDef RCC_OscInitStruct = {0};
                RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

                /** Supply configuration update enable
                */
                HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

                /** Configure the main internal regulator output voltage
                */
                __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

                while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

                /** Initializes the RCC Oscillators according to the specified parameters
                * in the RCC_OscInitTypeDef structure.
                */
                RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
                RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
                RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
                RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
                RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
                RCC_OscInitStruct.PLL.PLLM = 4;
                RCC_OscInitStruct.PLL.PLLN = 37;
                RCC_OscInitStruct.PLL.PLLP = 2;
                RCC_OscInitStruct.PLL.PLLQ = 4;
                RCC_OscInitStruct.PLL.PLLR = 2;
                RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
                RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
                RCC_OscInitStruct.PLL.PLLFRACN = 4096;
                if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
                {
                    EmbyDebug_ASSERT_FAIL();
                }

                /** Initializes the CPU, AHB and APB buses clocks
                */
                RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
                RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
                RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
                RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
                RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
                RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
                RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
                RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

                if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
                {
                    EmbyDebug_ASSERT_FAIL();
                }
                break;
            }
            case 280:
            {
                RCC_OscInitTypeDef RCC_OscInitStruct = {0};
                RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

                /** Supply configuration update enable
                */
                HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

                /** Configure the main internal regulator output voltage
                */
                __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

                while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

                /** Initializes the RCC Oscillators according to the specified parameters
                * in the RCC_OscInitTypeDef structure.
                */
                RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
                RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
                RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
                RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
                RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
                RCC_OscInitStruct.PLL.PLLM = 4;
                RCC_OscInitStruct.PLL.PLLN = 35;
                RCC_OscInitStruct.PLL.PLLP = 2;
                RCC_OscInitStruct.PLL.PLLQ = 4;
                RCC_OscInitStruct.PLL.PLLR = 2;
                RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
                RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
                RCC_OscInitStruct.PLL.PLLFRACN = 0;
                if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
                {
                    EmbyDebug_ASSERT_FAIL();
                }

                /** Initializes the CPU, AHB and APB buses clocks
                */
                RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
                RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
                RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
                RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
                RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
                RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
                RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
                RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

                if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
                {
                    EmbyDebug_ASSERT_FAIL();
                }
                break;
            }
            case 250:
            {
                RCC_OscInitTypeDef RCC_OscInitStruct = {0};
                RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

                /** Supply configuration update enable
                */
                HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

                /** Configure the main internal regulator output voltage
                */
                __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

                while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

                /** Initializes the RCC Oscillators according to the specified parameters
                * in the RCC_OscInitTypeDef structure.
                */
                RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
                RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
                RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
                RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
                RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
                RCC_OscInitStruct.PLL.PLLM = 4;
                RCC_OscInitStruct.PLL.PLLN = 31;
                RCC_OscInitStruct.PLL.PLLP = 2;
                RCC_OscInitStruct.PLL.PLLQ = 4;
                RCC_OscInitStruct.PLL.PLLR = 2;
                RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
                RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
                RCC_OscInitStruct.PLL.PLLFRACN = 2048;
                if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
                {
                    EmbyDebug_ASSERT_FAIL();
                }

                /** Initializes the CPU, AHB and APB buses clocks
                */
                RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
                RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
                RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
                RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
                RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
                RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
                RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
                RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

                if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
                {
                    EmbyDebug_ASSERT_FAIL();
                }
                break;
            }
            case 200:
            {
                RCC_OscInitTypeDef RCC_OscInitStruct = {0};
                RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

                /** Supply configuration update enable
                */
                HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

                /** Configure the main internal regulator output voltage
                */
                __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

                while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

                /** Initializes the RCC Oscillators according to the specified parameters
                * in the RCC_OscInitTypeDef structure.
                */
                RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
                RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
                RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
                RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
                RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
                RCC_OscInitStruct.PLL.PLLM = 4;
                RCC_OscInitStruct.PLL.PLLN = 25;
                RCC_OscInitStruct.PLL.PLLP = 2;
                RCC_OscInitStruct.PLL.PLLQ = 4;
                RCC_OscInitStruct.PLL.PLLR = 2;
                RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
                RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
                RCC_OscInitStruct.PLL.PLLFRACN = 0;
                if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
                {
                    EmbyDebug_ASSERT_FAIL();
                }

                /** Initializes the CPU, AHB and APB buses clocks
                */
                RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
                RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
                RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
                RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
                RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
                RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV4;
                RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
                RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

                if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
                {
                    EmbyDebug_ASSERT_FAIL();
                }
                break;
            }
            default:{

                RCC_OscInitTypeDef RCC_OscInitStruct = {0};
                RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

                /** Supply configuration update enable
                */
                //HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
                HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

                /** Configure the main internal regulator output voltage
                */
                __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

                while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

                /** Initializes the RCC Oscillators according to the specified parameters
                * in the RCC_OscInitTypeDef structure.
                */
                RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
                RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
                RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
                RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
                RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
                RCC_OscInitStruct.PLL.PLLM = 4;
                RCC_OscInitStruct.PLL.PLLN = 60;
                RCC_OscInitStruct.PLL.PLLP = 2;
                RCC_OscInitStruct.PLL.PLLQ = 2;
                RCC_OscInitStruct.PLL.PLLR = 2;
                RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
                RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
                RCC_OscInitStruct.PLL.PLLFRACN = 0;
                if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
                    EmbyDebug_ASSERT_FAIL();
                }

                /** Initializes the CPU, AHB and APB buses clocks
                */
                RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                                              | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
                RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
                RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
                RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
                RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
                RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
                RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
                RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

                if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
                    EmbyDebug_ASSERT_FAIL();
                }
                break;
            }
        }
    }
}
//override WEAK function of the SDK
    void init()
    {
        /* The STM32 CRC IP clock should be enabled to use the network runtime library */
        __HAL_RCC_CRC_CLK_ENABLE();
        __HAL_RCC_SYSCFG_CLK_ENABLE();
        HAL_Init();
        SCB_EnableDCache();
        SCB_EnableICache();

        SystemClock_Config(480);
    }

    void abort()
    {
//#ifdef BUILD_DEBUG
        while (1) {};
//#else

//#endif

    }

}
namespace EmbySystem {
    void cacheDEnable()
    {
        SCB_EnableDCache();
    }
    void cacheIEnable()
    {
        SCB_EnableICache();
    }
    void cacheDCleanAndInvalidate()
    {
        SCB_InvalidateDCache();
    }
}