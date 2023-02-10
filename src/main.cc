
#include "MainWorker.hh"
#include <EmbySystem/System.hh>
#include <EmbyThreading/Thread.hh>
#include "EmbyDebug/assert.hh"
#include  <exception>

namespace
{
//	/**
//	 * Run the main thread. We can't use the stack here because FreeRTOS gets
//	 * rid of it. Any object placed on the stack in the main function is simply
//	 * overwritten by tasks.
//	 *
//	 * This function exploits a C/C++ trick - object main, being declared
//	 * static is built the first time function runMainThread() is called and
//	 * then stays allocated and alive in the static memory space.
//	 *
//	 */
    void runMainThread()
    {
        static MainWorker main;
    }

    void terminate_handler()
    {
        EmbyDebug_ASSERT_FAIL();
    }
}


int main()
{
    std::set_terminate(terminate_handler);

    /*Perform init stuff related to the machine..board..and so on!*/
    EmbyMachine::init();

    /*start main thread*/
    runMainThread();

    EmbySystem::startScheduler();

    return 0;
}

