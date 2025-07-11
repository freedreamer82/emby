////
//// Created by lrignanese on 28/10/22.
////
//
//#include "EmbyMachine/WatchDog.hh"
//#include "EmbySystem/EmbySystem_Impl.hh"
//#include "EmbyDebug/assert.hh"
//
//extern "C"
//{
//#include "stm32l4xx.h"
//}
//
//using namespace EmbyMachine;
//using namespace EmbySystem;
//
//typedef enum
//{
//    WATCHDOG_STATUS_OK,                 /**< Operation successful. **/
//    WATCHDOG_STATUS_NOT_SUPPORTED,      /**< Operation not supported. **/
//    WATCHDOG_STATUS_INVALID_ARGUMENT,    /**< Invalid argument. **/
//    WATCHDOG_STATUS_ERROR,               /**< Operation fail. **/
//} watchdogStatus;
//
//typedef struct
//{
//    uint32_t timeout_ms;
//} watchdogConfig;
//
//extern "C"
//{
//#define MAX_IWDG_PR 0x6 // Max value of Prescaler_divider bits (PR) of Prescaler_register (IWDG_PR)
//#define MAX_IWDG_RL 0xFFFUL // Max value of Watchdog_counter_reload_value bits (RL) of Reload_register (IWDG_RLR).
//
//// Convert Prescaler_divider bits (PR) of Prescaler_register (IWDG_PR) to a prescaler_divider value.
//#define PR2PRESCALER_DIV(PR_BITS) \
//   (4UL << (PR_BITS))
//
//#define LSI_VALUE  32000U      /*!< LSI Typical Value in Hz*/
//
//// Convert Prescaler_divider bits (PR) of Prescaler_register (IWDG_PR)
//// and Watchdog_counter_reload_value bits (RL) of Reload_register (IWDG_RLR)
//// to a timeout value [ms].
//#define PR_RL2UINT64_TIMEOUT_MS(PR_BITS, RL_BITS) \
//       ((PR2PRESCALER_DIV(PR_BITS)) * (RL_BITS) * 1000ULL / (LSI_VALUE))
//
//// Convert Prescaler_divider bits (PR) of Prescaler_register (IWDG_PR) and a timeout value [ms]
//// to Watchdog_counter_reload_value bits (RL) of Reload_register (IWDG_RLR)
//#define PR_TIMEOUT_MS2RL(PR_BITS, TIMEOUT_MS) \
//       (((TIMEOUT_MS) * (LSI_VALUE) / (PR2PRESCALER_DIV(PR_BITS)) + 999UL) / 1000UL)
//
//#define MAX_TIMEOUT_MS_UINT64 PR_RL2UINT64_TIMEOUT_MS(MAX_IWDG_PR, MAX_IWDG_RL)
//#if (MAX_TIMEOUT_MS_UINT64 > UINT32_MAX)
//#define MAX_TIMEOUT_MS UINT32_MAX
//#else
//#define MAX_TIMEOUT_MS (MAX_TIMEOUT_MS_UINT64 & 0xFFFFFFFFUL)
//#endif
//
//#define INVALID_IWDG_PR ((MAX_IWDG_PR) + 1) // Arbitrary value used to mark an invalid PR bits value.
//
//// Pick a minimal Prescaler_divider bits (PR) value suitable for given timeout.
//static uint8_t pick_min_iwdg_pr(const uint32_t timeout_ms)
//{
//    for (uint8_t pr = 0; pr <= MAX_IWDG_PR; pr++)
//    {
//        // Check that max timeout for given pr is greater than
//        // or equal to timeout_ms.
//        if (PR_RL2UINT64_TIMEOUT_MS(pr, MAX_IWDG_RL) >= timeout_ms)
//        {
//            return pr;
//        }
//    }
//    return INVALID_IWDG_PR;
//}
//
//IWDG_HandleTypeDef IwdgHandle;
//
//static watchdogStatus hal_watchdog_init(const watchdogConfig *config)
//{
//    const uint8_t pr = pick_min_iwdg_pr(config->timeout_ms);
//    if (pr == INVALID_IWDG_PR)
//    {
//        return WATCHDOG_STATUS_INVALID_ARGUMENT;
//    }
//    const uint32_t rl = PR_TIMEOUT_MS2RL(pr, config->timeout_ms);
//
//    IwdgHandle.Instance = IWDG;
//
//    IwdgHandle.Init.Prescaler = pr;
//    IwdgHandle.Init.Reload = rl;
//#if defined IWDG_WINR_WIN
//    IwdgHandle.Init.Window = IWDG_WINDOW_DISABLE;
//#endif
//
//    if (HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
//    {
//        return WATCHDOG_STATUS_ERROR;
//    }
//
//    return WATCHDOG_STATUS_OK;
//}
//
//void hal_watchdog_kick(void)
//{
//    HAL_IWDG_Refresh(&IwdgHandle);
//}
//
//watchdogStatus hal_watchdog_stop(void)
//{
//    return WATCHDOG_STATUS_NOT_SUPPORTED;
//}
//
//uint32_t hal_watchdog_get_reload_value(void)
//{
//    // Wait for the Watchdog_prescaler_value_update bit (PVU) of
//    // Status_register (IWDG_SR) to be reset.
//    while (READ_BIT(IWDG->SR, IWDG_SR_PVU))
//    {
//    }
//    // Read Prescaler_divider bits (PR) of Prescaler_register (IWDG_PR).
//    const uint8_t pr = (IWDG->PR & IWDG_PR_PR_Msk) >> IWDG_PR_PR_Pos;
//
//    // Wait for the Watchdog_counter_reload_value_update bit (RVU) of
//    // Status_register (IWDG_SR) to be reset.
//    while (READ_BIT(IWDG->SR, IWDG_SR_RVU))
//    {
//    }
//    // Read Watchdog_counter_reload_value bits (RL) of Reload_register (IWDG_RLR).
//    const uint32_t rl = (IWDG->RLR & IWDG_RLR_RL_Msk) >> IWDG_RLR_RL_Pos;
//
//    return PR_RL2UINT64_TIMEOUT_MS(pr, rl);
//}
//
//static uint32_t hal_watchdog_get_max_timeout(void)
//{
//    return MAX_TIMEOUT_MS;
//}
//};
//
//class STM32WatchDog
//{
//public:
//    static STM32WatchDog &get();
//
//    bool isRunning()
//    {
//        return m_isRunning;
//    }
//
//    void setTimout(uint32_t);
//
//    ~STM32WatchDog()
//    {};
//
//    bool start();
//
//    bool stop();
//
//    void kick();
//
//private:
//    STM32WatchDog()
//    {
//        init();
//    };
//
//    bool init();
//
//    bool m_isRunning;
//    uint32_t m_timeout;
//};
//
//STM32WatchDog &STM32WatchDog::get()
//{
//    static STM32WatchDog instance;
//    return instance;
//}
//
//bool STM32WatchDog::init()
//{
//    m_isRunning = false;
//    return true;
//}
//
//void STM32WatchDog::setTimout(uint32_t timeout)
//{
//    EmbyDebug_ASSERT(timeout <= hal_watchdog_get_max_timeout());
//    EmbyDebug_ASSERT(timeout > 0);
//    m_timeout = timeout;
//}
//
//bool STM32WatchDog::start()
//{
//    EnterCriticalSection();
//    if (m_isRunning)
//    {
//        ExitCriticalSection();
//        return false;
//    }
//    watchdogConfig config;
//    config.timeout_ms = m_timeout;
//    watchdogStatus sts = hal_watchdog_init(&config);
//
//    if (sts == WATCHDOG_STATUS_OK)
//    {
//        m_isRunning = true;
//    }
//    else
//    {
//        m_isRunning = false;
//    }
//    ExitCriticalSection();;
//    return m_isRunning;
//}
//
//bool STM32WatchDog::stop()
//{
//    watchdogStatus status;
//    bool out = true;
//    EnterCriticalSection();
//    if (m_isRunning)
//    {
//        status = hal_watchdog_stop();
//        if (status != WATCHDOG_STATUS_OK)
//        {
//            out = false;
//        }
//        else
//        {
//            m_isRunning = false;
//        }
//
//    }
//    else
//    {
//        out = false;
//    }
//    ExitCriticalSection();
//    return out;
//}
//
//void STM32WatchDog::kick()
//{
//    EnterCriticalSection();
//    hal_watchdog_kick();
//    ExitCriticalSection();
//}
//
//void WatchDog::setUp(EmbyTime::Millis timeout)
//{
//    STM32WatchDog::get().setTimout(uint32_t(timeout));
//}
//
//void WatchDog::kick()
//{
//    STM32WatchDog::get().kick();
//}
//
//bool WatchDog::enable()
//{
//    return STM32WatchDog::get().start();
//}
//
//bool WatchDog::disable()
//{
//    return STM32WatchDog::get().stop();
//}
//
//bool WatchDog::isRunning()
//{
//    return STM32WatchDog::get().isRunning();
//}
