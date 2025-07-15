#include <EmbyMachine/EmbyMachine.hh>
#include <cstdlib>

namespace EmbyMachine
{

    // Override WEAK function of the SDK
    void init()
    {
    }

    void abort()
    {
        while (1) {}
    }

}
