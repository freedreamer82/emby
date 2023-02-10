/**
* @addtogroup Application
* @{
* @file ConsoleWorker.cc
* @author Massimiliano Pagani
* @version 1.0
* @date 19/12/2014
*
*/

#include <EmbyConsole/ConsoleWorker.hh>
#include <EmbySystem/System.hh>


ConsoleWorker* g_consoleWorker = nullptr;

ConsoleWorker::ConsoleWorker() :
			   m_thread( this, "Cons", STACK_SIZE, PRIORITY  ),
			   m_consoles{}
{
}

ConsoleWorker::~ConsoleWorker()
{
}

bool
ConsoleWorker::addConsole( EmbyConsole::Console* console)
{
	m_consoles.push_back(console);
	return true;
}

bool ConsoleWorker::isRegistered(EmbyConsole::Console* console)
{
        auto proc =  std::find(m_consoles.begin(),m_consoles.end(),console);
        if(proc != m_consoles.end() )
        {
                return true;
        }
        return false;
}

bool
ConsoleWorker::removeConsole( EmbyConsole::Console* console)
{
    bool retval = false;
    auto proc =  std::find(m_consoles.begin(),m_consoles.end(),console);
    if(proc != m_consoles.end() )
    {
    	m_consoles.erase(proc);
    	retval = true;
    }
    return retval;
}

void
ConsoleWorker::doWork()
{
	for(EmbyConsole::Console* console : m_consoles)
	{
		console->doStep();
	}
	EmbySystem::delayMs( 20 );
}


ConsoleWorker& ConsoleWorker::get()
{
	if(g_consoleWorker == nullptr)
	{
		g_consoleWorker = new ConsoleWorker();
		EmbyDebug_ASSERT_CHECK_NULL_PTR(g_consoleWorker);
	}
	return *g_consoleWorker;
}
