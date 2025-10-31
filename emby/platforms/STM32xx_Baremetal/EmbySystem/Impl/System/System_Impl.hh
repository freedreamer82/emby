
#if !defined(SYSTEM_IMPL_H)
#define SYSTEM_IMPL_H

#include <stdbool.h>
#include <stdint.h>

namespace EmbySystem
{
    struct SystemUniqueId
    {
        uint32_t m_id[4];
    };
} // namespace EmbySystem
#endif
