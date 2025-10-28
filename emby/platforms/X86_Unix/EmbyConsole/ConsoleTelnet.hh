#if !defined( EMBYCONSOLE_TELNET_CONSOLE_HH )
#define EMBYCONSOLE_TELNET_CONSOLE_HH

/* Includes ------------------------------------------------------------------*/

#include <EmbyConsole/Console.hh>
#include <EmbyConsole/ConsoleWorker.hh>
#include <EmbyLog/Log.hh>
#include <EmbyLog/LogClass.hh>
#include <EmbyThreading/Thread.hh>

/* Exported types ------------------------------------------------------------*/
namespace EmbyConsole
{

    class ConsoleTelnetConnection : public Console
    {
    public:

        ConsoleTelnetConnection(EmbyLog::LogMask mask, int fd);

        static int setSocketNonblocking(int fd);

        int getSocketId()
        {
            return m_socketId;
        }

        bool
        isActive()
        {
            return m_isActive;
        }

    protected:

        virtual void transmit(char const *msg, size_t length);

        virtual size_t read(char *buffer, size_t length);

    private:
        int m_socketId;
        bool m_isActive;
    };

    class ConsoleTelnet : private ConsoleWorker
    {

    public:

        ConsoleTelnet(EmbyLog::LogMask mask, uint16_t listenPort, uint16_t maxConnections);

        virtual ~ConsoleTelnet()
        {};

        void setApplicationCommands(ConsoleCommands *commands)
        {
            m_appCommands = commands;
        }

        ConsoleCommands *getApplicationCommands()
        {
            return m_appCommands;
        }

    protected:

        virtual void doWork();

    private:
        int m_listenSocket;
        uint16_t m_listenPort;
        //std::vector<ConsoleTelnetConnection>     m_clients;
        EmbyLog::LogMask m_consoleLogmask;
        uint16_t m_maxConnections;
        ConsoleCommands *m_appCommands;


        bool startServerSocket(uint16_t port);

    };
}

#endif
