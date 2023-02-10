

#if !defined( MAINWORKER_HH )
#define MAINWORKER_HH

#include <EmbyThreading/Worker.hh>
#include <EmbyThreading/Thread.hh>

using namespace EmbyThreading;

class MainWorker : public EmbyThreading::Worker
{
public:
    MainWorker();

    virtual ~MainWorker();

private:
    [[noreturn]] virtual void doWork();

    static size_t const STACK_SIZE = 600;
    static int const PRIORITY = EmbyThreading::PRIORITY_LOWEST;

    EmbyThreading::Thread m_thread;
};

#endif
///@}


