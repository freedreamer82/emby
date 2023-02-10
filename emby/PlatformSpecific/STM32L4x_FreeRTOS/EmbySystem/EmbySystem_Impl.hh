
#if !defined(SYSTEM_IMPL_H)
#define SYSTEM_IMPL_H

#include <stdbool.h>
#include <stdint.h>

namespace EmbySystem
{
//    #define EmbySystem_BEGIN_CRITICAL_SECTION
//    #define EmbySystem_END_CRITICAL_SECTION

    struct SystemUniqueId
    {
        uint32_t m_id[4];
    };
} // namespace EmbySystem
#endif
