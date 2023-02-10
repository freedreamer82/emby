#if !defined(SYSTEM_IMPL_H)
#define SYSTEM_IMPL_H

#include <stdbool.h>
#include <stdint.h>
#include "platform/mbed_critical.h"

namespace EmbySystem
{
#define EmbySystem_EnterCriticalSection() core_util_critical_section_enter();
#define EmbySystem_ExitCriticalSection() core_util_critical_section_exit();

#define EmbySystem_BEGIN_CRITICAL_SECTION \
    {                                     \
        core_util_critical_section_enter();

#define EmbySystem_END_CRITICAL_SECTION \
    core_util_critical_section_exit();  \
    }

    //	#define EmbySystem_DISABLE_INTERRUPT	    taskDISABLE_INTERRUPTS()
    //
    //	#define EmbySystem_ENABLE_INTERRUPT	        taskENABLE_INTERRUPTS()

    struct SystemUniqueId
    {
        uint32_t m_id[4];
    };
} // namespace EmbySystem
#endif
