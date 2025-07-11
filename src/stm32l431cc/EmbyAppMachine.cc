#include <EmbyMachine/EmbyMachine.hh>
#include <cstdlib>

#include <cstring>


namespace EmbyMachine
{

#if defined(EMBY_BUILD_ARM) &&  defined(EMBY_BUILD_STM32L4x_FreeRTOS)
extern "C"
{
    #include "stm32l4xx.h"

    /** @addtogroup STM32L4xx_System_Private_Variables
  * @{
  */
    /* The SystemCoreClock variable is updated in three ways:
        1) by calling CMSIS function SystemCoreClockUpdate()
        2) by calling HAL API function HAL_RCC_GetHCLKFreq()
        3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
           Note: If you use this function to configure the system clock; then there
                 is no need to call the 2 first functions listed above, since SystemCoreClock
                 variable is updated automatically.
    */
    uint32_t SystemCoreClock = 4000000U;

    const uint8_t AHBPrescTable[16] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U};
    const uint8_t APBPrescTable[8] = {0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U};
    const uint32_t MSIRangeTable[12] = {100000U, 200000U, 400000U, 800000U, 1000000U, 2000000U, \
                                      4000000U, 8000000U, 16000000U, 24000000U, 32000000U, 48000000U};


    void SystemInit(void)
        {
    #if defined(USER_VECT_TAB_ADDRESS)
            /* Configure the Vector Table location -------------------------------------*/
      SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
    #endif

            /* FPU settings ------------------------------------------------------------*/
    #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
            SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));  /* set CP10 and CP11 Full Access */
    #endif
        }

    //define stuff for ARM STM32 VALUE
    void SystemCoreClockUpdate(void)
    {
        uint32_t tmp, msirange, pllvco, pllsource, pllm, pllr;

        /* Get MSI Range frequency--------------------------------------------------*/
        if ((RCC->CR & RCC_CR_MSIRGSEL) == 0U)
        { /* MSISRANGE from RCC_CSR applies */
            msirange = (RCC->CSR & RCC_CSR_MSISRANGE) >> 8U;
        }
        else
        { /* MSIRANGE from RCC_CR applies */
            msirange = (RCC->CR & RCC_CR_MSIRANGE) >> 4U;
        }
        /*MSI frequency range in HZ*/
        msirange = MSIRangeTable[msirange];

        /* Get SYSCLK source -------------------------------------------------------*/
        switch (RCC->CFGR & RCC_CFGR_SWS)
        {
            case 0x00:  /* MSI used as system clock source */
                SystemCoreClock = msirange;
                break;

            case 0x04:  /* HSI used as system clock source */
                SystemCoreClock = HSI_VALUE;
                break;

            case 0x08:  /* HSE used as system clock source */
                SystemCoreClock = HSE_VALUE;
                break;

            case 0x0C:  /* PLL used as system clock  source */
                /* PLL_VCO = (HSE_VALUE or HSI_VALUE or MSI_VALUE/ PLLM) * PLLN
                   SYSCLK = PLL_VCO / PLLR
                   */
                pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC);
                pllm = ((RCC->PLLCFGR & RCC_PLLCFGR_PLLM) >> 4U) + 1U;

                switch (pllsource)
                {
                    case 0x02:  /* HSI used as PLL clock source */
                        pllvco = (HSI_VALUE / pllm);
                        break;

                    case 0x03:  /* HSE used as PLL clock source */
                        pllvco = (HSE_VALUE / pllm);
                        break;

                    default:    /* MSI used as PLL clock source */
                        pllvco = (msirange / pllm);
                        break;
                }
                pllvco = pllvco * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 8U);
                pllr = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLR) >> 25U) + 1U) * 2U;
                SystemCoreClock = pllvco / pllr;
                break;

            default:
                SystemCoreClock = msirange;
                break;
        }
        /* Compute HCLK clock frequency --------------------------------------------*/
        /* Get HCLK prescaler */
        tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4U)];
        /* HCLK clock frequency */
        SystemCoreClock >>= tmp;
    }


} //externC
#endif





//override WEAK function of the SDK
    void init()
    {

    }

    void
    abort()
    {
#ifdef BUILD_DEBUG
        while (1) {};
#else

#endif

    }


}
