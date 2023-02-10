
#if !defined( EMBYCONSOLE_CONSOLE_HH )
#define EMBYCONSOLE_CONSOLE_HH

/* Includes ------------------------------------------------------------------*/
#include <cstdint>
#include <cstring>
#include <EmbyThreading/Mutex.hh>
#include <cstdarg>
#include <EmbyLibs/BitMask.hh>
#include <EmbyDebug/assert.hh>
#include <EmbyLibs/String.hh>
#include <EmbyLog/LogLevel.hh>
#include <EmbyLog/LogProcessor.hh>
#include <EmbyLog/LogProcessorQueue.hh>
#include <EmbyLog/LogClass.hh>
#include <EmbyConsole/ConsoleCommands.hh>
#include <EmbyLibs/IOutput.hh>
#include <EmbyLibs/CharBuffer.hh>


namespace EmbyConsole
{

#ifndef EMBY_CFG_CONSOLE_LINE_CURSOR
#define     EMBY_CFG_CONSOLE_LINE_CURSOR                                                           ">"
#endif


#ifndef EMBY_CFG_CONSOLE_COMMAND_BUFF_SIZE
#define     EMBY_CFG_CONSOLE_COMMAND_BUFF_SIZE                                                           80
#endif


#ifndef EMBY_CFG_CONSOLE_LOGIN_ROOT_PSW
#define     EMBY_CFG_CONSOLE_LOGIN_ROOT_PSW                              "root"
#endif

#ifndef EMBY_CFG_CONSOLE_LOGIN_GUEST_PSW
#define     EMBY_CFG_CONSOLE_LOGIN_GUEST_PSW                             "guest"
#endif

#ifndef EMBY_CFG_CONSOLE_USER_LOGIN_TIMEOUT_SEC
#define     EMBY_CFG_CONSOLE_USER_LOGIN_TIMEOUT_SEC                              3
#endif

#define     EOL                                                                     "\r\n"


// compares two strings in compile time constant fashion
    constexpr int getMaxPasswordLen(char const *guest, char const *root)
    {
        return strlen(root) > strlen(guest) ? strlen(root) + strlen(EOL) : strlen(guest) + strlen(EOL);
    }

//check the max size of the root/guest anc add \r\n (2 bytes)
#ifndef EMBY_CFG_MAX_CONSOLE_PWD_SIZE

    static constexpr int EMBY_CFG_MAX_CONSOLE_PWD_SIZE = getMaxPasswordLen(EMBY_CFG_CONSOLE_LOGIN_GUEST_PSW,
                                                                           EMBY_CFG_CONSOLE_LOGIN_ROOT_PSW);
//    #if 0 == c_strcmp(EMBY_CFG_CONSOLE_LOGIN_GUEST_PSW,EMBY_CFG_CONSOLE_LOGIN_ROOT_PSW )
//        #define EMBY_CFG_MAX_CONSOLE_PWD_SIZE 3
//        #define DISPLAY_VALUE2(x) #x
//        #define DISPLAY_VALUE(x) DISPLAY_VALUE2(x)
//        #pragma message( "My Value  = " DISPLAY_VALUE(EMBY_CFG_MAX_CONSOLE_PWD_SIZE) )
//    #endif
#endif


#define     CONSOLE_MAX_CMD_ARGS                                                        8
#define     NUM_LINES_CLEAR                                                           100

#define     CONSOLE_FLAG_ESCAPE                                 ((uint16_t)BitMask_BIT(0))
#define     CONSOLE_FLAG_LINE                                   ((uint16_t)BitMask_BIT(1))
#define     CONSOLE_FLAG_IS_LOGGED                              ((uint16_t)BitMask_BIT(2))
#define     CONSOLE_FLAG_PRINT_WELCOME                          ((uint16_t)BitMask_BIT(3))
#define     CONSOLE_FLAG_ASK_LOGIN                              ((uint16_t)BitMask_BIT(4))
#define     CONSOLE_FLAG_TIMEOUT                                ((uint16_t)BitMask_BIT(5))
#define     CONSOLE_FLAG_ASK_PASSWORD                           ((uint16_t)BitMask_BIT(6))
#define     CONSOLE_FLAG_TIME_EXPIRED                           ((uint16_t)BitMask_BIT(7))
#define     CONSOLE_FLAG_DISABLE_ECHO                           ((uint16_t)BitMask_BIT(8))
#define     CONSOLE_FLAG_PRINT_CLASS_LOG                        ((uint16_t)BitMask_BIT(9))
#define     CONSOLE_FLAG_LOG_FILTER_ENABLE_TYPE                 ((uint16_t)BitMask_BIT(10))
#define     CONSOLE_FLAG_HANDLE_COMMON_COMMANDS                 ((uint16_t)BitMask_BIT(11))
#define     CONSOLE_FLAG_HANDLE_APP_COMMANDS                    ((uint16_t)BitMask_BIT(12))
#define     CONSOLE_FLAG_PRINT_PROMPT                            ((uint16_t)BitMask_BIT(13))
#define     CONSOLE_FLAG_TIME_WAS_EXPIRED                        ((uint16_t)BitMask_BIT(14))
#define     CONSOLE_FLAG_EMPTY_COMMAND_NOT_RECOGNIZED            ((uint16_t)BitMask_BIT(15))


#define     VT100_SAVECURSOR       "\e7"  /* Save cursor and attrib */
#define    VT100_RESTORECURSOR    "\e8"  /* Restore cursor pos and attribs */
#define    VT100_SETWIN_CLEAR     "\e[r" /* Clear scrollable window size */
#define    VT100_CLEAR_SCREEN     "\e[2J" /* Clear screen */
#define    VT100_CLEAR_LINE       "\e[2K" /* Clear this whole line */
#define    VT100_CLEAR_LINE_FROM_CURSOR "\e[0K" //# Clear line from cursor to the end
#define    VT100_CLEAR_LINE_UPTO_CURSOR "\e[1K"// # Clear line upto the cursor
#define    VT100_RESET_TERMINAL   "\ec"

#define     VT100_MOVE_LINE_UP(_N_)       "\e["##(_N_)"A"  //VT100_CURSOR_UP move 1 "\e[3A" # up - moves 3 lines up
#define     VT100_MOVE_LINE_DOWN(_N_)     "\e["##(_N_)"B"
#define     VT100_MOVE_CHAR_RIGTH(_N_)    "\e["##(_N_)"C"
#define     VT100_MOVE_CHAR_LEFT(_N_)     "\e["##(_N_)##"D"

#define    VT100_CURSOR_BACK       "\e[D\e[0K"  //move left and clear
#define    VT100_CURSOR_LEFT       "\e[D"
#define    VT100_CURSOR_RIGTH       "\e[C"
#define    VT100_CURSOR_UP           "\e[A"
#define    VT100_CURSOR_DOWN       "\e[B"

#define    VT100_SHOW_CURSOR      "\e[?25h"
#define    VT100_HIDE_DOWN          "\e[?25l"


    /**
     * Abstract Console Class
     */
    class Console : public EmbyLibs::IOutput
    {
    public:

        enum class FloatPrecision : uint32_t
        {
            DEC1 = 10,
            DEC2 = 100,
            DEC3 = 1000,
            DEC4 = 10000,
            DEC5 = 100000,
            DEC6 = 1000000,

        };


        static char constexpr
        const*
        CONSOLE_lineCursor = EMBY_CFG_CONSOLE_LINE_CURSOR;

        /**
         * Create a Console with a Logger.
         * @param mask : mask of the Logger;
         * @param askLogin : if true login session will be required
         */
        Console(EmbyLog::LogMask mask, bool askLogin = true);

        /**
        * Create a Console without a Logger.
        * @param askLogin : if true login session will be required
        */
        Console(bool askLogin = true);

        virtual ~Console();

        /**
         * Console doStep , must be called often to process the
         * Console state machine
         */
        void
        doStep()
        {
            doWork();
        }

        /**
         * Ask Login to the Console
         * @param askLogin : if true login session will be required
         */
        void
        askLogin(bool askLogin);

        /**
        * Check if any USER( root or guest) is logged
        * @return : true if an usser is logged false otherwise.
        */
        bool
        isUserLogged() const;

        /**
        * Set a welcome msg in the console
        */
        void
        setWelcomeMsg(char const *msg);

        /**
        * Set a welcome msg in the console
        */
        void
        setApplicationCommands(ConsoleCommands *commands)
        {
            m_appCommands = commands;
        }

        ConsoleCommands *
        getApplicationCommands()
        {
            return m_appCommands;
        }

        void
        setEcho(bool value)
        {
            if (!value)
            {
                BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_DISABLE_ECHO);
            }
            else
            {
                BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_DISABLE_ECHO);
            }
        }

        /*
         * Read a flag timeout expired and clear it..
         * */
        bool
        isTimeoutExpired();

        void
        setFlag(uint16_t flag)
        {
            BitMask_SET_MASK(m_flagMask, flag);
        }

        void
        clearFlag(uint16_t flag)
        {
            BitMask_CLR_MASK(m_flagMask, flag);
        }

        void
        setEndOfLineChar(char eol)
        {
            m_eol = eol;
        }

        char
        getEndOfLineChar()
        {
            return m_eol;
        }

        bool
        isLoggerEnabled();

        bool
        enableLogger();

        bool
        disableLogger();

        void clearBuffer();

        const char *
        getWelcomeMsg() const
        {
            return m_welcomeMsg;
        }

        /**
         * SetUp the inactivity timeout , if you want to log out from console
         * automatically after a timeout
         * @param sec : seconds
         */
        void
        setInactivityTimeSec(uint16_t sec);

        uint32_t
        getInactivityTimeSec() const
        {
            return m_inactivityTimeSec;
        }

        uint32_t
        getLastActivityTimeSec() const
        {
            return m_lastSysTimeReadSec;
        }


        bool
        isTimeoutExpired() const
        {
            return (!BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED) &&
                    BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_TIME_EXPIRED));
        }

        void
        printHex(void *p, size_t len);

        void
        printf(const char *fmt, ...);


        void printFloat(float f, FloatPrecision p);


        /**
         * Prints a container that can be iterated.
         *
         * @param begin an iterator to the first character to print.
         * @param end an iterator to one past last character to print.
         */
        template<typename It>
        void print(It begin, It end);


        /**
         * Prints a LightString to the console.
         *
         * @param text the text that will be printed onto the console.
         */
        void print(EmbyLibs::String const &text);

        void print(char const *text);

        void print(char ch);

        void println();

        void println(char const *text);

        void println(EmbyLibs::String const &text);

        void clearScreen();

        char *getLastCommand()
        {
            return m_last.data();
        }

        void
        setDefaultErrorString();

        void
        setErrorString(const CommandErrorElem *table)
        {
            m_errorCode = table;
        }

    protected:

        // This function is already abstract from IOutput interface
        //  virtual void 	transmit( char const* msg, size_t length) = 0;

        virtual size_t read(char *buffer, size_t length) = 0;

        virtual bool login();

        virtual void doWork();

        void reset();

#if !defined( EMBY_CONSOLE_DISABLE_MUTEX )

        EmbyThreading::Mutex *getMutex()
        {
            return &m_mutex;
        }

#endif

    private:

        bool
        trimCommand();

        bool
        isLogAllowed(EmbyLog::LogMessage &log);

        void
        init(bool askLogin);

        CommandErrorCode
        printHelp(char *command = nullptr);

        void
        vprintf(const char *fmt, va_list args);

        CommandErrorCode
        commandHandler();

        CommandErrorCode
        memoryDump(char const *const *args, int argsNo);

        bool
        setLogMask(char *command, char **args, int argsNo);

        void
        processChar(char ch);

        const char *
        getErrorString(CommandErrorCode code);

#if !defined (INTELLISENSE_DISABLED)

        void
        handleIntellisense();

#endif

#if !defined( EMBY_CONSOLE_DISABLE_MUTEX )
        EmbyThreading::Mutex m_mutex;
#endif
        EmbyLog::LogProcessorQueue *m_logger;
        ConsoleCommands *m_appCommands;
        std::vector<char> *m_msg;
        char const *m_welcomeMsg;
        EmbyLibs::CharBuffer <EMBY_CFG_MAX_CONSOLE_PWD_SIZE> *m_password;
        EmbyLibs::CharBuffer<EMBY_CFG_CONSOLE_COMMAND_BUFF_SIZE> m_cmd;
        EmbyLibs::CharBuffer<EMBY_CFG_CONSOLE_COMMAND_BUFF_SIZE> m_last;
        EmbyLog::LogClass m_logClassFilter;
        uint32_t m_lastSysTimeReadSec;
        User m_userLevel;
        uint16_t m_flagMask;
        uint32_t m_inactivityTimeSec;
        uint8_t m_eol;
        const CommandErrorElem *m_errorCode;
    };


    // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // ::::::::::::::::::::::::::::::::::::::::::::::: template implementation :::
    // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    template<typename It>
    void Console::print(It begin, It end)
    {
        while (begin != end)
        {
            print(*begin);
            ++begin;
        }
    }


}

#endif /* <HEADER_UNIQUE_SYMBOL_H> */

/**
 * @}
 */
