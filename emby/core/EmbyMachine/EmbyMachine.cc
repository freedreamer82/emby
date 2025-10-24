#include <EmbyMachine/EmbyMachine.hh>

__attribute__((weak))  void  EmbyMachine::abort()
{
    std::abort();
}


__attribute__((weak))  void  EmbyMachine::init()
{

}