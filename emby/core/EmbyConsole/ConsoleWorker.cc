
#include <EmbyConsole/ConsoleWorker.hh>
#include <EmbySystem/System.hh>

ConsoleWorker* g_consoleWorker = nullptr;

ConsoleWorker::ConsoleWorker()
        : m_thread(this, "Cons", EMBY_CFG_CONSOLE_THREAD_STACK_SIZE, PRIORITY)
{
}

ConsoleWorker::~ConsoleWorker()
{
}

bool ConsoleWorker::addConsole(EmbyConsole::Console* console)
{
    m_consoles.push_back(console);
    return true;
}

bool ConsoleWorker::removeConsole(EmbyConsole::Console* console)
{
    bool retval = false;
    auto proc   = std::find(m_consoles.begin(), m_consoles.end(), console);
    if (proc != m_consoles.end())
    {
        m_consoles.erase(proc);
        retval = true;
    }
    return retval;
}

void ConsoleWorker::doWork()
{
    for (EmbyConsole::Console* console : m_consoles)
    {
        console->doStep();
    }
    EmbySystem::delayMs(20);
}

ConsoleWorker& ConsoleWorker::get()
{
    if (g_consoleWorker == nullptr)
    {
        g_consoleWorker = new ConsoleWorker();
        EmbyDebug_ASSERT_CHECK_NULL_PTR(g_consoleWorker);
    }
    return *g_consoleWorker;
}
