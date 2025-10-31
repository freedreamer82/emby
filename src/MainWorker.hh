

#if !defined( MAINWORKER_HH )
#define MAINWORKER_HH

#include <EmbyThreading/Worker.hh>
#include <EmbyThreading/Thread.hh>
#include <EmbySystem/SystemError.hh>
#include <EmbySystem/ErrorCode.hh>

using namespace EmbyThreading;

class MainWorker: public EmbyThreading::Worker
{
public:
    MainWorker();

    virtual ~MainWorker();

private:
    [[noreturn]] virtual void doWork();

    bool onErrors(EmbySystem::ErrorCode *err, EmbySystem::SystemError::Status status);

    static size_t const STACK_SIZE = 600;
    static auto const PRIORITY = EmbyThreading::Thread::Priority::Normal;

    EmbyThreading::Thread m_thread;
};

#endif
///@}


