/**
 * @addtogroup Application
 * @{
 * @file ConsoleWorker.hh
 * @author Massimiliano Pagani
 * @version 1.0
 * @date 19/12/2014
 *
 */

#if !defined( CONSOLEWORKER_HH )
#define CONSOLEWORKER_HH

#include <EmbyThreading/Worker.hh>
#include <EmbyConsole/Console.hh>
#include <EmbyThreading/Thread.hh>
#include <vector>

/** 
 * ConsoleWorker is the thread for handling console.
 */

class ConsoleWorker : public EmbyThreading::Worker
{
	public:


                bool isRegistered(EmbyConsole::Console* console);
		bool addConsole( EmbyConsole::Console* console);
		bool removeConsole( EmbyConsole::Console* console);
		/**
		 * @{
		 * Singleton methods.NOT inherited from EmbySigleton to avoid multiple inheritance
		 */
		static ConsoleWorker& get();

		bool start()
		{
		    return m_thread.start();
		}

	private:


		static size_t const STACK_SIZE	 	= 500;
		static int    const PRIORITY        =  EmbyThreading::PRIORITY_LOWEST;

		ConsoleWorker( ConsoleWorker const& copy );
		ConsoleWorker& operator=( ConsoleWorker const& copy );
		EmbyThreading::Thread 				 m_thread;

	protected:

		std::vector<EmbyConsole::Console*>   m_consoles;
		virtual void doWork();

		ConsoleWorker();
		virtual ~ConsoleWorker();
};

#endif
///@}


