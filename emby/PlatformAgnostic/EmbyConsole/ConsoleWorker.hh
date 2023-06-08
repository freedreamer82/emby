
#if !defined( CONSOLEWORKER_HH )
#define CONSOLEWORKER_HH

#include <EmbyThreading/Worker.hh>
#include <EmbyConsole/Console.hh>
#include <EmbyThreading/Thread.hh>
#include <vector>


#ifndef EMBY_CFG_CONSOLE_THREAD_STACK_SIZE
#define     EMBY_CFG_CONSOLE_THREAD_STACK_SIZE   500
#endif


/** 
 * ConsoleWorker is the thread for handling console.
 */

class ConsoleWorker : public EmbyThreading::Worker
{
public:


    bool isRegistered(EmbyConsole::Console *console);

    bool addConsole(EmbyConsole::Console *console);

    bool removeConsole(EmbyConsole::Console *console);

    /**
     * @{
     * Singleton methods.NOT inherited from EmbySigleton to avoid multiple inheritance
     */
    static ConsoleWorker &get();

    bool start()
    {
        return m_thread.start();
    }

private:

    static EmbyThreading::Thread::Priority const PRIORITY = EmbyThreading::Thread::Priority::VeryLow;

    ConsoleWorker(ConsoleWorker const &copy);

    ConsoleWorker &operator=(ConsoleWorker const &copy);

    EmbyThreading::Thread m_thread;

protected:

    std::vector<EmbyConsole::Console *> m_consoles;

    virtual void doWork();

    ConsoleWorker();

    virtual ~ConsoleWorker();
};

#endif
///@}


