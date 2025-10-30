// Default weak implementations for basic system functions.
// Platforms may provide strong definitions to override these.

#include <cstdlib>
#include "EmbySystem/System.hh"

namespace EmbySystem
{

    __attribute__((weak)) void EnterCriticalSection()
    {
        // default: no-op
    }

    __attribute__((weak)) void ExitCriticalSection()
    {
        // default: no-op
    }

    __attribute__((weak)) void abort()
    {
        ::abort();
    }

}

