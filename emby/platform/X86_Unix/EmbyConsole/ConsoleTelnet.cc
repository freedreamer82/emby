#include <EmbyConsole/ConsoleTelnet.hh>
#include <EmbySystem/System.hh>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
//#include <sys/types.h>
#include <netinet/in.h>
//#include <sys/wait.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <signal.h>

#define WELCOME_MSG EOL\
"============================================================" EOL\
"           ______________________________________           " EOL\
"  ________|                                      |_______  " EOL\
"  \\       |          Welcome  to  Emby           |      / " EOL\
"   \\      |               Console                |     / " EOL\
"   /      |______________________________________|     \\ " EOL\
"  /__________)                                (_________\\ " EOL\
" " EOL\
"============================================================" EOL EOL

EmbyLog_MODULE_LOG_CLASS("conS")

namespace EmbyConsole
{

//	static void err_handler(int signum)
//	{
//	    if (signum == SIGPIPE)
//	    {
//	    }
//	}

	ConsoleTelnetConnection::
	ConsoleTelnetConnection(EmbyLog::LogMask mask , int fd) :
						EmbyConsole::Console (mask) , m_socketId (fd)
	{
		EmbyDebug_ASSERT(fd != -1);
		ConsoleTelnetConnection::setSocketNonblocking(fd);
		// instal sigpipe handler
		m_isActive = true;
	//	signal(SIGPIPE,err_handler);
	}



	/*----------------------------------------------------------------------
	 Portable function to set a socket into nonblocking mode.
	 Calling this on a socket causes all future read() and write() calls on
	 that socket to do only as much as they can immediately, and return
	 without waiting.
	 If no data can be read or written, they return -1 and set errno
	 to EAGAIN (or EWOULDBLOCK).
	 Thanks to Bjorn Reese for this code.
	----------------------------------------------------------------------*/
	int
	ConsoleTelnetConnection::setSocketNonblocking(int fd)
	{
	    int flags;

	    /* If they have O_NONBLOCK, use the Posix way to do it */
	#if defined(O_NONBLOCK)
	    /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
	    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
	        flags = 0;
	    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	#else
	    /* Otherwise, use the old way of doing it */
	    flags = 1;
	    return ioctl(fd, FIOBIO, &flags);
	#endif
	}

	void
	ConsoleTelnetConnection::transmit(char const* msg, size_t length)
	{
		int nwrite = write(m_socketId, msg, length);
		if (nwrite < 0)
		{
			//log_error("write failed");
		}
	}

	size_t  ConsoleTelnetConnection::read(char* buffer, size_t length)
	{
		int retval = ::read(m_socketId, buffer, length);
		if(retval ==  0)
		{
			m_isActive  = false;
		}
		return (retval == -1) ?  0 : retval;
	}


	ConsoleTelnet:: ConsoleTelnet(EmbyLog::LogMask mask , uint16_t listenPort , uint16_t maxConnections) :
				m_listenSocket(-1),
				m_listenPort(listenPort),
				m_consoleLogmask(mask),
	            m_maxConnections(maxConnections)
	{
		EmbyDebug_ASSERT(m_listenPort > 0);

		/*get instance of the worker*/
		get();

		if ( ! startServerSocket(m_listenPort) )
		{
			log_error("ERROR opening socket");
		}
	}

	bool
	ConsoleTelnet::
	startServerSocket(uint16_t port)
	{
		bool retval = false;

		if (port != 0 )
		{
			m_listenSocket = socket(AF_INET, SOCK_STREAM, 0);// SO_REUSEPORT | SO_REUSEADDR);
			if(m_listenSocket != -1)
			{
				int on;
				if (setsockopt(m_listenSocket,SOL_SOCKET,SO_REUSEADDR,(const char *) &on,sizeof(on)) == -1)
				{
				    perror("Error Socket Server");
				    exit(1);
				}

				struct sockaddr_in serv_addr;

				memset(&serv_addr, '0', sizeof(serv_addr));

				serv_addr.sin_family = AF_INET;
				serv_addr.sin_addr.s_addr = INADDR_ANY;
				serv_addr.sin_port = htons(m_listenPort);

				if (bind(m_listenSocket, (struct sockaddr *) &serv_addr,
						  sizeof(serv_addr)) < 0)
				{
					retval = false;
				}
				else
				{
					listen(m_listenSocket,m_maxConnections);
				   /* Force the network socket into nonblocking mode */
					retval = ConsoleTelnetConnection :: setSocketNonblocking(m_listenSocket) == 0;
					//retval = true;
				}
			}
		}
		return retval;
	}


	void
	ConsoleTelnet::doWork()
	{
		/*Blocking until new connection*/
		int fd = accept(m_listenSocket, NULL, NULL);
		if (fd != -1)
		{
			log_debug("New connection incoming");
			EmbyConsole::Console*  client = new ConsoleTelnetConnection(m_consoleLogmask, fd);
			client->setEcho(false);
			client->setWelcomeMsg(WELCOME_MSG);
			client->setEndOfLineChar('\n');
			if (m_appCommands)
            {
                client->setApplicationCommands(m_appCommands);
            }
			addConsole( client );
		}

		auto  consoleNo =m_consoles.size();
		if(consoleNo != 0)
		{
			for (size_t i = 0; i < consoleNo; i++)
			{
				Console * c = m_consoles.at(i);
				ConsoleTelnetConnection* client = dynamic_cast<ConsoleTelnetConnection*>(c);
				if (client)
				{
					if (!client->isActive())
					{
						log_debug("Connection dropped..");
						close(client->getSocketId() );
						removeConsole(client);
						delete(client);
					}
				}
			}
		}
		ConsoleWorker::doWork();

	}
}
