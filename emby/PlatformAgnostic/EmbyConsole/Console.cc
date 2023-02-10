#include <EmbyConsole/Console.hh>
#include <EmbySystem/System.hh>
#include <EmbyLog/LogMessage.hh>
#include <EmbyLog/LogBus.hh>
#include <EmbyLibs/StringUtils.hh>
#include <EmbyLibs/OutputFormatter.hh>
#include <EmbyLog/LogClassUtils.hh>
#include <EmbyThreading/Thread.hh>
#include <EmbyThreading/ScopedMutex.hh>

namespace EmbyConsole
{

    static const EmbyConsole::CommandErrorElem errorCode[] =
            {
                    {.code = NO_ERR, .str = ""},
                    {.code = CMD_NOT_FOUND, .str = "Command Not Found!"},
                    {.code = BAD_FORMAT, .str = "Bad Format!"}};

    static const CommandInfo basiCommands[] =
            {
                    {.cmd = "logs", .userLevel = USER_GUEST, .cmdInfo = "logs <mask> <all,none,debug...> | logs <enable|disable> <LogClass> | logs reset"},
                    {.cmd = "uptime", .userLevel = USER_GUEST, .cmdInfo = "uptime Ms"},
                    {.cmd = "timeout", .userLevel = USER_ROOT, .cmdInfo = "timeout [sec]"},
                    {.cmd = "clear", .userLevel = USER_ROOT, .cmdInfo = "clear screen"},
                    {.cmd = "whoAmI", .userLevel = USER_ROOT, .cmdInfo = "user"},
                    {.cmd = "help", .userLevel = USER_GUEST, .cmdInfo = "help"},
                    {.cmd = "devmem", .userLevel = USER_ROOT, .cmdInfo = " dump memory <start> <stop> [b]"},
            };

#if !defined(EMBY_CONSOLE_DISABLE_MUTEX)
#define SCOPED_MUTEX() EmbyThreading::ScopedMutex lock_##__LINE__(m_mutex)
#else
#define SCOPED_MUTEX()
#endif

#define BACKSPACE 0x08
#define TAB 0x09
#define DEL 0x7F
#define CANCEL 0x18
#define ARROW 0x1B

#define MAX_COMMANDS_TIPS 3

#define EOL_CHAR '\r'

    // static const char eol[sizeof(EOL)] = EOL;

    Console::Console(bool isLoginConsole) : m_mutex(),
                                            m_logger(nullptr),
                                            m_appCommands(nullptr),
                                            m_msg(nullptr),
                                            m_welcomeMsg(nullptr),
                                            m_password(nullptr),
                                            m_logClassFilter(EmbyLog::LOG_CLASS_NONE),
                                            m_userLevel(USER_NONE),
                                            m_inactivityTimeSec(0),
                                            m_eol(EOL_CHAR),
                                            m_errorCode(errorCode)
    {
        init(isLoginConsole);
    }

    Console::Console(EmbyLog::LogMask mask, bool isLoginConsole) :

            m_mutex(),
            m_logger(new EmbyLog::LogProcessorQueue(mask)),
            m_appCommands(nullptr),
            m_msg(nullptr),
            m_welcomeMsg(nullptr),
            m_password(nullptr),
            m_logClassFilter(EmbyLog::LOG_CLASS_NONE),
            m_userLevel(USER_NONE),
            m_inactivityTimeSec(0),
            m_eol(EOL_CHAR),
            m_errorCode(errorCode)
    {
        EmbyDebug_ASSERT_CHECK_NULL_PTR(m_logger);
        init(isLoginConsole);
    }

    Console::~Console()
    {
        reset();
        if (m_logger)
        {
            delete (m_logger);
            m_logger = nullptr;
        }
        //on dtor we release Always the password even if one time malloc
        if (m_password)
        {
            delete (m_password);
            m_password = nullptr;
        }
    }

    void
    Console::init(bool isLoginConsole)
    {

        EmbyDebug_ASSERT(m_cmd.capacity() > 0);
        EmbyDebug_ASSERT(m_last.capacity() > 0);

        setDefaultErrorString();

        BitMask_CLEAR_ALL(m_flagMask);

        /*Console starts as logged..if you want to set a login you can with a method..*/
        BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED);
        BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_HANDLE_COMMON_COMMANDS);
        BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_HANDLE_APP_COMMANDS);
        BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_PROMPT);

        askLogin(isLoginConsole);

        reset();
    }

    void
    Console::reset()
    {
        SCOPED_MUTEX();

        BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_ESCAPE);
        if (!BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_ASK_LOGIN))
        {
            /**if not login set as logged */
            BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED);
        }
        else
        {
            /**if not login set as logged */
            BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED);
            BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_ASK_PASSWORD);
        }

        if (m_welcomeMsg)
        {
            BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_WELCOME);
        }

        BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_CLASS_LOG);

        /*init last Reaf*/
        m_lastSysTimeReadSec = EmbySystem::upTimeMs() / 1000;

        m_cmd.clear();
        m_last.clear();
        m_logClassFilter = EmbyLog::LOG_CLASS_NONE;

        if (m_password)
        {
#ifdef EMBY_CFG_CONSOLE_FREE_MEMORY_PASSWORD
            delete (m_password);
            m_password = nullptr;
#endif
        }

        disableLogger();
    }

    bool
    Console::isLoggerEnabled()
    {
        SCOPED_MUTEX();
        bool retval = false;
        if (m_logger && EmbyLog::LogBus::get().isRegistered(m_logger))
        {
            retval = true;
        }
        return retval;
    }

    bool
    Console::disableLogger()
    {
        SCOPED_MUTEX();
        bool retval = false;
        if (m_logger && EmbyLog::LogBus::get().isRegistered(m_logger))
        {
            EmbyLog::LogBus::get().unregisterProcessor(m_logger);
            m_logger->clearQueue();
            retval = true;
        }
        return retval;
    }

    void Console::clearBuffer()
    {
        EmbySystem::EnterCriticalSection();
        m_cmd.clear();
        m_last.clear();
        memset(m_cmd.data(), 0, m_cmd.capacity());
        memset(m_last.data(), 0, m_last.capacity());
        EmbySystem::ExitCriticalSection();
    }

    bool
    Console::enableLogger()
    {
        SCOPED_MUTEX();
        bool retval = false;
        if (m_logger && !EmbyLog::LogBus::get().isRegistered(m_logger))
        {
            EmbyLog::LogBus::get().registerProcessor(m_logger);
            retval = true;
        }
        return retval;
    }

    bool
    Console::isUserLogged() const
    {
        return BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED);
    }

    void
    Console::setInactivityTimeSec(uint16_t sec)
    {
        SCOPED_MUTEX();
        if (sec != 0)
        {
            BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_TIMEOUT);
            m_lastSysTimeReadSec = EmbySystem::upTimeMs() / 1000;
        }
        else
        {
            BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_TIMEOUT);
        }
        m_inactivityTimeSec = sec;
    }

    void
    Console::setWelcomeMsg(char const *msg)
    {
        SCOPED_MUTEX();
        BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_WELCOME);
        m_welcomeMsg = msg;
    }

    void
    Console::askLogin(bool isLoginConsole)
    {
        SCOPED_MUTEX();

        if (isLoginConsole)
        {
            BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_ASK_LOGIN);
            BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED);
            BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_ASK_PASSWORD);
        }
        else
        {
            //   BitMask_SET_MASK(m_flagMask,CONSOLE_FLAG_IS_LOGGED);
            BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED);
            BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_ASK_LOGIN);
            BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_ASK_PASSWORD);
            m_userLevel = USER_ROOT;
        }

        if (!isLoginConsole)
        {
            enableLogger();
        }
    }

    void
    Console::print(EmbyLibs::String const &text)
    {
        print(text.begin(), text.end());
    }

    void
    Console::print(char const *text)
    {
        transmit(text, strlen(text));
    }

    void
    Console::printf(const char *fmt, ...)
    {
        va_list args;

        va_start(args, fmt);

        vprintf(fmt, args);

        va_end(args);
    }

    void Console::println(char const *text)
    {
        transmit(text, strlen(text));
        println();
    }

    void
    Console::vprintf(const char *fmt, va_list args)
    {
        SCOPED_MUTEX();
        EmbyLibs::vprintf(*this, fmt, args);
    }

    void Console::print(char ch)
    {
        transmit(&ch, sizeof(ch));
    }

    void Console::println()
    {
        transmit(EOL, sizeof(EOL) - 1);
    }

    void Console::println(EmbyLibs::String const &text)
    {
        print(text);
        println();
    }

    bool
    Console::login()
    {
        bool isResetNeeded = false;

        uint32_t upTimeSec = EmbySystem::upTimeMs() / 1000;

        if (!BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED))
        {
            char c;

            uint8_t readByte = read((char *) &c, 1);
            if (readByte && m_password)
            {
                if (m_password->size() >= m_password->capacity())
                {
                    memset(m_password->data(), 0, m_password->capacity());
                }
                else
                {
                    m_password->push_back(c);
                    m_lastSysTimeReadSec = upTimeSec;
                }
#ifdef REPLY_ON_LOGIN
                if(c!= m_eol)
                {
                    print('*');
                }
#endif
            }

            if (upTimeSec - m_lastSysTimeReadSec > EMBY_CFG_CONSOLE_USER_LOGIN_TIMEOUT_SEC)
            {
                isResetNeeded = true;
            }

            if (m_password && m_password->back() == m_eol)
            {
                /**you can't choose psw runtime...too much resources...*/

                if (strncmp(m_password->data(), EMBY_CFG_CONSOLE_LOGIN_ROOT_PSW,
                            strlen(EMBY_CFG_CONSOLE_LOGIN_ROOT_PSW)) == 0)
                {
                    println();
                    println("Welcome root!");
                    m_userLevel = USER_ROOT;
                    BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED);
                    memset(m_password->data(), 0, m_password->capacity());
                    enableLogger();
                }
                else if (strncmp(m_password->data(), EMBY_CFG_CONSOLE_LOGIN_GUEST_PSW,
                                 strlen(EMBY_CFG_CONSOLE_LOGIN_GUEST_PSW)) == 0)
                {
                    println();
                    println("Welcome!");
                    m_userLevel = USER_GUEST;
                    BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED);
                    memset(m_password->data(), 0, m_password->capacity());
                    enableLogger();
                }
                else
                {
                    /*ask password..*/
                    m_password->clear();
                    memset(m_password->data(), 0, m_password->max_size());
                    BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_ASK_PASSWORD);
                }
                m_lastSysTimeReadSec = upTimeSec;
            }
        }
        else
        { /*Do nothing*/
        }

        if (isResetNeeded)
        {
            m_userLevel = USER_NONE;
            if (m_password)
            {
                m_password->clear();
                memset(m_password->data(), 0, m_password->max_size());
            }
        }
        return BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED);
    }

    void
    Console::clearScreen()
    {
        for (uint8_t i = 0; i < NUM_LINES_CLEAR; i++)
        {
            println();
        }
    }

    bool
    Console::isLogAllowed(EmbyLog::LogMessage &log)
    {
        bool retval = false;
        EmbyLog::LogClass logClass = log.getLogClass();
        if (m_logClassFilter == EmbyLog::LOG_CLASS_NONE)
        {
            retval = true;
        }
        else if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_LOG_FILTER_ENABLE_TYPE) && logClass == m_logClassFilter)
        {
            retval = true;
        }
        else if (!BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_LOG_FILTER_ENABLE_TYPE) && logClass != m_logClassFilter)
        {
            retval = true;
        }
        return retval;
    }

    CommandErrorCode
    Console::printHelp(char *command)
    {
        CommandErrorCode retval = CMD_NOT_FOUND;
        bool printAll = (command == nullptr);

        for (uint8_t tableCmdIdx = 0; tableCmdIdx < 2; tableCmdIdx++)
        {
            const CommandInfo *tableCommands = nullptr;
            uint16_t tableElemSize = 0;
            if (tableCmdIdx == 0)
            {
                tableCommands = basiCommands;
                tableElemSize = sizeof(basiCommands) / sizeof(CommandInfo);
            }
            else if (tableCmdIdx == 1 && m_appCommands != nullptr)
            {
                tableCommands = m_appCommands->getCommandsDescription();
                tableElemSize = m_appCommands->getCommandsDescriptionSize() / sizeof(CommandInfo);
            }

            if (tableCommands)
            {
                for (uint16_t i = 0; i < tableElemSize; i++)
                {
                    if (!printAll)
                    {
                        if (std::strncmp(tableCommands[i].cmd, m_cmd.data(), strlen(m_cmd.data())) == 0)
                        {
                            retval = NO_ERR;
                            println(tableCommands[i].cmdInfo);
                            break;
                        }
                    }
                    else
                    {
                        if (m_userLevel >= tableCommands[i].userLevel)
                        {
                            /*print according to your userLevel*/
                            printf("+ %s "
                            EOL
                            " # %s #"
                            EOL,
                                    tableCommands[i].cmd, tableCommands[i].cmdInfo);
                            EmbySystem::delayMs(10);
                            printf("---------------------------------------"
                            EOL);
                            retval = NO_ERR;
                        }
                    }
                }
            }
        }
        return retval;
    }

    bool
    Console::setLogMask(char *command, char **args, int argsNo)
    {
        (void) command;
        bool retval = false;
        if (argsNo == 0)
        {
            printf("Log mask is %d\r\n", m_logger->getLevelMask());
            if (m_logClassFilter != EmbyLog::LOG_CLASS_NONE &&
                BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_LOG_FILTER_ENABLE_TYPE))
            {
                print("Enabled only Module: ");
                println(EmbyLog::LogClass_toString(m_logClassFilter));
            }
            else if (m_logClassFilter != EmbyLog::LOG_CLASS_NONE &&
                     !BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_LOG_FILTER_ENABLE_TYPE))
            {
                print("Disabled Module: ");
                println(EmbyLog::LogClass_toString(m_logClassFilter));
            }
            retval = true;
        }
        else if (argsNo == 1 && std::strcmp(args[0], "reset") == 0)
        {
            m_logClassFilter = EmbyLog::LOG_CLASS_NONE;
            retval = true;
        }
        else if (argsNo == 2)
        {
            retval = true;
            EmbyLog::LogMask logLevel = EmbyLog::LevelMask::None;
            if (argsNo == 2 && std::strcmp(args[0], "mask") == 0)
            {

                if (std::strcmp(args[1], "info") == 0)
                {
                    logLevel = EmbyLog::LevelMask::From_Info;
                }
                else if (std::strcmp(args[1], "debug") == 0)
                {
                    logLevel = EmbyLog::LevelMask::From_Debug;
                }
                else if (std::strcmp(args[1], "trace") == 0)
                {
                    logLevel = EmbyLog::LevelMask::From_Trace;
                }
                else if (std::strcmp(args[1], "warning") == 0)
                {
                    logLevel = EmbyLog::LevelMask::From_Warning;
                }
                else if (std::strcmp(args[1], "error") == 0)
                {
                    logLevel = EmbyLog::LevelMask::From_Error;
                }
                else if (std::strcmp(args[1], "fatal") == 0)
                {
                    logLevel = EmbyLog::LevelMask::From_Fatal;
                }
                else if (std::strcmp(args[1], "none") == 0)
                {
                    logLevel = EmbyLog::LevelMask::None;
                }
                else if (std::strcmp(args[1], "all") == 0)
                {
                    logLevel = EmbyLog::LevelMask::All;
                }
                else
                {
                    retval = false;
                }

                if (retval)
                {
                    printf("Mask changed!\r\n");
                    m_logger->setLevelMask(logLevel);
                }
            }
            else
            {
                if (m_logger)
                {
                    if (argsNo == 2 && std::strcmp(args[0], "enable") == 0)
                    {
                        BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_LOG_FILTER_ENABLE_TYPE);
                        m_logClassFilter = EmbyLog::LogCLass_fromNameToNumberId(args[1]);
                        print("Enable Logs only  on module: ");
                        println(EmbyLog::LogClass_toString(m_logClassFilter));
                        retval = true;
                    }
                    else if (argsNo == 2 && std::strcmp(args[0], "disable") == 0)
                    {
                        BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_LOG_FILTER_ENABLE_TYPE);;
                        m_logClassFilter = EmbyLog::LogCLass_fromNameToNumberId(args[1]);
                        print("Disabled Logs of module: ");
                        println(EmbyLog::LogClass_toString(m_logClassFilter));
                        retval = true;
                    }
                    else
                    {
                        m_logClassFilter = EmbyLog::LOG_CLASS_NONE;
                    }
                }
                else
                {
                    printf("Logger not enabled!\r\n");
                }
            }
        }
        return retval;
    }

    CommandErrorCode
    Console::memoryDump(char const *const *args, int argsNo)
    {
        if (argsNo < 2)
        {
            return BAD_FORMAT;
        }

        bool isByteFormat = (argsNo == 3 && std::strcmp(args[2], "b") == 0);

        auto start = reinterpret_cast<uint32_t const *>(::strtol(args[0], NULL, 16));
        auto stop = reinterpret_cast<uint32_t const *>(::strtol(args[1], NULL, 16));

        int n = 0;
        while (start < stop)
        {
            if (n % 4 == 0)
            {
                printf("%08x: ", reinterpret_cast<uint32_t const *>(start));
            }
            uint32_t value = *start;
            if (isByteFormat)
            {
                for (unsigned i = 0; i < sizeof(uint32_t); ++i)
                {
                    printf("%02x ", (value & 0xFF));
                    value >>= 8;
                }
            }
            else
            {
                printf("%08x", value);
            }

            if (++n % 4 == 0)
            {
                // carriage return
                println();
            }
            else
            {
                print(" ");
            }
            start++;
        }
        println();
        return NO_ERR;
    }

    bool
    Console::trimCommand()
    {

        while (m_cmd.size() && (m_cmd.data()[m_cmd.size() - 1] == '\r' ||
                                m_cmd.data()[m_cmd.size() - 1] == '\n'))
        {
            m_cmd.pop_back();
        }
        if (m_cmd.full())
        {
            // error line too long.
            m_cmd.clear();
            return false;
        }
        else
        {
            m_cmd.push_back(0);
            return true;
        }
    }

    CommandErrorCode
    Console::commandHandler()
    {
        CommandErrorCode retval = BAD_FORMAT;
        char *args[CONSOLE_MAX_CMD_ARGS];

        int argsNum = 0;
        args[0] = EmbyLibs::strtok(m_cmd.data(), " ");
        char *tokenPtr = nullptr;

        do
        {
            args[argsNum] = EmbyLibs::strtok(NULL, " ");
            tokenPtr = args[argsNum];
            if (tokenPtr)
            {
                argsNum++;
            }
        } while (tokenPtr != nullptr && argsNum < CONSOLE_MAX_CMD_ARGS);

        if (!m_cmd[0])
        {
            if (!BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_EMPTY_COMMAND_NOT_RECOGNIZED))
            {
                // null command
                retval = NO_ERR;
            }
        }
        else if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_HANDLE_COMMON_COMMANDS) &&
                 argsNum == 1 && std::strcmp(args[0], "help") == 0)
        {
            retval = printHelp(m_cmd.data());
        }
        else if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_HANDLE_COMMON_COMMANDS) &&
                 argsNum == 0 && std::strcmp(m_cmd.data(), "clear") == 0)
        {
            clearScreen();
            retval = NO_ERR;
        }
        else if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_HANDLE_COMMON_COMMANDS) &&
                 argsNum == 0 && std::strcmp(m_cmd.data(), "uptime") == 0)
        {
            uint64_t now = EmbySystem::upTimeMs();
            uint32_t nowSec = now / 1000;

            uint32_t days = (uint32_t) nowSec / 86400;
            uint32_t hours = (uint32_t)(nowSec / 3600) - (days * 24);
            uint32_t mins = (uint32_t)(nowSec / 60) - (days * 1440) - (hours * 60);
            uint32_t secs = (uint32_t) nowSec % 60;

            printf("Sec from boot: %d \r\n", nowSec);
            printf("Time from Boot: %d day, %d hours, %d min, %d sec\r\n", days, hours, mins, secs);
            retval = NO_ERR;
        }
        else if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_HANDLE_COMMON_COMMANDS) &&
                 std::strcmp(m_cmd.data(), "timeout") == 0)
        {
            if (argsNum == 0)
            {
                printf("Timeout sec: %d", m_inactivityTimeSec);
                println();
                retval = NO_ERR;
            }
            else if (argsNum == 1)
            {
                setInactivityTimeSec(atoi(args[0]));
                retval = NO_ERR;
            }
        }
        else if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_HANDLE_COMMON_COMMANDS) &&
                 argsNum == 0 && std::strcmp(m_cmd.data(), "help") == 0)
        {
            println("######   LIST OF CONSOLE'S CMD  #######");
            println();

            printHelp();

            println();
            retval = NO_ERR;
        }
        else if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_HANDLE_COMMON_COMMANDS) &&
                 argsNum == 0 && strcmp(m_cmd.data(), "whoAmI") == 0)
        {
            printf("User Level = %d. "
            EOL, m_userLevel);
            retval = NO_ERR;
        }
        else if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_HANDLE_COMMON_COMMANDS) &&
                 std::strcmp(m_cmd.data(), "logs") == 0)
        {
            retval = setLogMask(m_cmd.data(), args, argsNum) ? NO_ERR : BAD_FORMAT;
        }
        else if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_HANDLE_COMMON_COMMANDS) &&
                 std::strcmp(m_cmd.data(), "devmem") == 0)
        {
            retval = memoryDump(args, argsNum);
        }
        else
        {
            if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_HANDLE_APP_COMMANDS) &&
                m_appCommands != nullptr)
            {
                //        		auto len = m_cmd.size();
                //        		char* trimmedCmd = m_cmd.data();
                //        		for( uint32_t i = 0 ; i < len ; i++  )
                //        		{
                //        			if( trimmedCmd &&
                //        			   ( *trimmedCmd == '\n' ||
                //        				 *trimmedCmd == '\r'))
                //        			{
                //        				//removed /n /t in front of string
                //        				trimmedCmd++;
                //        			}
                //        		}

                retval = m_appCommands->doCommand(*this, m_cmd.data(), args, argsNum);
            }
            else
            {
                retval = CMD_NOT_FOUND;
            }
        }
        return retval;
    }

    void
    Console::printHex(void *p, size_t len)
    {
        size_t i = 0;
        while (i < len)
        {
            printf("%2.2x", ((uint8_t *) p)[i++]);
        }
    }

#if !defined(INTELLISENSE_DISABLED)

    void
    Console::handleIntellisense()
    {
        // int commonChars = 0;
        CommandInfo *tips[MAX_COMMANDS_TIPS];
        int tipsIdx = 0;
        memset(tips, 0, sizeof(tips));
        uint16_t cmdSize = m_cmd.size();
        bool moreCommandsAvailable = false;

        for (uint8_t tableCmdIdx = 0; tableCmdIdx < 2; tableCmdIdx++)
        {
            const CommandInfo *tableCommands = nullptr;
            uint16_t tableElemSize = 0;
            if (tableCmdIdx == 0)
            {
                tableCommands = basiCommands;
                tableElemSize = sizeof(basiCommands) / sizeof(CommandInfo);
            }
            else if (tableCmdIdx == 1 && m_appCommands != nullptr)
            {
                tableCommands = m_appCommands->getCommandsDescription();
                tableElemSize = m_appCommands->getCommandsDescriptionSize() / sizeof(CommandInfo);
            }

            if (tableCommands)
            {
                for (uint16_t i = 0; i < tableElemSize; i++)
                {
                    if (cmdSize <= strlen(tableCommands[i].cmd))
                    {
                        int commonChars = std::strncmp(tableCommands[i].cmd, m_cmd.data(), cmdSize);
                        if (commonChars == 0)
                        {
                            if (tipsIdx < MAX_COMMANDS_TIPS)
                            {
                                tips[tipsIdx++] = (CommandInfo * ) & tableCommands[i];
                            }
                            else
                            {
                                moreCommandsAvailable = true;
                            }
                        }
                        else
                        {
                            commonChars = 0;
                        }
                    }
                }
            }
        }

        if (tipsIdx == 1)
        {
            /*only 1 command found...*/
            m_cmd.clear();
            memset(m_cmd.data(), 0, m_cmd.capacity());
            for (uint16_t j = 0; j < strlen(tips[0]->cmd); j++)
            {
                m_cmd.push_back(tips[0]->cmd[j]);
            }

            if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_PROMPT))
            {
                printf("\e[%dD", cmdSize + 1); // move cursor to left + 1 is prompt
            }
            else
            {
                printf("\e[%dD", cmdSize);
            }
            print(VT100_CLEAR_LINE_FROM_CURSOR);

            if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_PROMPT))
            {
                print(CONSOLE_lineCursor);
            }
            print(tips[0]->cmd);
        }
        else
        {
            /*more than one... show them!*/
            for (uint16_t i = 0; i < tipsIdx; i++)
            {
                if (i == 0)
                {
                    println();
                }
                print(tips[i]->cmd);
                print("  ");
                if ((i + 1) == tipsIdx)
                {
                    if (moreCommandsAvailable)
                    {
                        print("...");
                    }
                    println();
                    if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_PROMPT))
                    {
                        print(CONSOLE_lineCursor);
                    }
                    print(m_cmd.data());
                }
            }
        }
    }

#endif

    void
    Console::processChar(char c)
    {
        uint8_t len = m_cmd.size();
        //
        //        if(len == 0 && ( c == '\r' ||  c == '\n' ) )
        //        {
        //        	if (m_eol == c && BitMask_GET_MASK(m_flagMask,CONSOLE_FLAG_PRINT_PROMPT) )
        //			{
        //        		printPrompt();
        //			}
        //        	return;
        //        }

        if (c == m_eol)
        {
            if (!trimCommand())
            {
                println(getErrorString(BAD_FORMAT));
                return;
            }

            if (m_cmd.size() != 0)
            {
                // save last command
                // strncpy(m_last.data(), m_cmd.data(), m_cmd.capacity());
                m_last = m_cmd;
            }

            if (m_eol != '\n')
            {
                println();
            }

            BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_ESCAPE);

            if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_HANDLE_COMMON_COMMANDS) &&
                std::strcmp(m_cmd.data(), "exit") == 0)
            {
                BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_ASK_LOGIN);
                reset();
            }
            else
            {
                CommandErrorCode err = commandHandler();

                const char *errorStr = getErrorString(err);
                //                switch (err)
                //                {
                //                    case CMD_NOT_FOUND:
                //                        println( strlnegetErrorString(CMD_NOT_FOUND));
                ////                        println( "Command Not Found!");
                //                    break;
                //                    case BAD_FORMAT:
                //                    	println(getErrorString(BAD_FORMAT));
                //                        //println("Bad Format!");
                //                    break;
                //                    case NO_ERR:
                //						println(getErrorString(NO_ERR));
                //						//println("Bad Format!");
                //						break;
                //                    default:
                //                    break;
                //                }
                if (strlen(errorStr))
                {
                    println(errorStr);
                }

                if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_PROMPT))
                {
                    print(CONSOLE_lineCursor);
                }
                m_cmd.clear();
                memset(m_cmd.data(), 0, m_cmd.capacity());
            }
        }
        else
        {
            switch (c)
            {
                case BACKSPACE:
                case DEL:
                    if (!m_cmd.empty())
                    {
                        print(VT100_CURSOR_BACK);
                        // m_cmd.at( m_cmd.size() - 1) = 0;
                        m_cmd.pop_back();
                    }
                    break;

                case ARROW:
                    BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_ESCAPE);
                    break;
#if !defined(INTELLISENSE_DISABLED)
                case TAB:

                    handleIntellisense();

                    break;
#endif
                case 'A':
                    if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_ESCAPE))
                    {
                        m_cmd = m_last;
                        print(VT100_CLEAR_LINE);
                        if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_PROMPT))
                        {
                            printf("\e[%dD", m_cmd.size() + 1);
                            print(CONSOLE_lineCursor);
                        }
                        else
                        {
                            printf("\e[%dD", m_cmd.size() + 1);
                        }
                        print(m_cmd.data());
                        BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_ESCAPE);
                        break;
                    }
                    else
                    { /* do nothing */
                    }
                    // fall through

                default:
                    if (!BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_ESCAPE) &&
                        (len + 1u < m_cmd.capacity()))
                    {
                        if (!BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_DISABLE_ECHO))
                        {
                            print(c);
                        }
                        m_cmd.push_back(c);
                    }
                    else
                    { /* do nothing */
                    }
                    break;
            }
        }
    }

    void
    Console::doWork()
    {
        //  Log* log = nullptr;
        uint32_t upTimeSec = 0;

#ifdef SYSLOG_USE_CLOCK
        Date date;
        TimeOfDay time;
#endif

        if (m_welcomeMsg != nullptr &&
            BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_WELCOME))
        {
            /*Print welcome msg!!*/
            print(m_welcomeMsg);
            println();
            if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_PROMPT))
            {
                /*first prompt*/
                if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_PROMPT))
                {
                    print(CONSOLE_lineCursor);
                }
            }
            BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_PRINT_WELCOME);
        }

        if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_ASK_LOGIN) &&
            !BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED))
        {
            /** Ask login!*/
            if (m_password == nullptr)
            {
                m_password = new EmbyLibs::CharBuffer<EMBY_CFG_MAX_CONSOLE_PWD_SIZE>;
            }

            if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_ASK_PASSWORD))
            {
                println("Password?");
            }
            BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_ASK_PASSWORD);
        }

        if (!BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED) &&
            login())
        {
            if (m_password != nullptr)
            {
#ifdef EMBY_CFG_CONSOLE_FREE_MEMORY_PASSWORD
                /*does not need anymore the password memory..*/
                delete (m_password);
                m_password = nullptr;
#endif
            }
            BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED);
            BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_ASK_PASSWORD);
            if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_PROMPT))
            {
                print(CONSOLE_lineCursor);
            }
        }

        upTimeSec = EmbySystem::upTimeMs() / 1000;

        EmbyLog::LogMessage log;

        if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED))
        {
            /*timeout not expired!*/
            BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_TIME_EXPIRED);

            if (m_logger && !m_logger->deQueue(&log))
            {
                /*do nothing*/
            }
            else
            {
                bool isALog2print = isLogAllowed(log);

                if (isALog2print)
                {
                    if (log.getTimestamp() != 0)
                    {
                        uint32_t timestamp = static_cast<int32_t>(log.getTimestamp() & 0x7FFFFFFF);
                        print(EmbyLibs::to_string(timestamp));
                        print(" ");
                    }
#if !defined(EMBY_ARCH_LINUX_X86)
                    static char const levels[] = {'-', 'F', 'E', 'W', 'I', 'D', 'T'};
#else
                    static char *const levels[] = {"-", "FATAL", "ERROR", "WARNING", "INFO", "DEBUG", "TRACE"};
#endif
                    print(levels[static_cast<int>(log.getLogLevel())]);
                    print(' ');
                    EmbyLog::LogClass logClass = log.getLogClass();
                    if (logClass != EmbyLog::LOG_CLASS_NONE &&
                        BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_CLASS_LOG))
                    {
                        print(EmbyLog::LogClass_toString(logClass));
                        print(": ");
                    }
                    println(log.getText());
                    if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_PRINT_PROMPT))
                    {
                        print(CONSOLE_lineCursor);
                    }
                }
            }

            //            upTimeSec = EmbySystem::upTimeMs()/1000;

            char buffer[20];
            size_t length;

            while ((length = read(buffer, sizeof(buffer))) > 0)
            {
                /** Activity time update*/
                m_lastSysTimeReadSec = upTimeSec;

                for (size_t i = 0; i < length; ++i)
                {
                    processChar(buffer[i]);
                }
            }

        } // is logged

        if ((BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_TIMEOUT)) &&
            BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED) &&
            (upTimeSec > m_lastSysTimeReadSec) &&
            (upTimeSec - m_lastSysTimeReadSec) > m_inactivityTimeSec)
        {

            printf("Expired %d sec Console Timeout!"
            EOL, m_inactivityTimeSec);

            reset();
            BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_TIME_EXPIRED);
            BitMask_SET_MASK(m_flagMask, CONSOLE_FLAG_TIME_WAS_EXPIRED);
            BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_IS_LOGGED);
        }

        EmbyThreading::Thread::yield();
        //        EmbySystem::delayMs(10);
    }

    const char *
    Console::getErrorString(CommandErrorCode code)
    {
        uint32_t tableElemSize = sizeof(errorCode) / sizeof(CommandErrorElem);
        for (uint16_t i = 0; i < tableElemSize; i++)
        {
            if (m_errorCode[i].code == code)
            {
                return m_errorCode[i].str;
                break;
            }
        }
        return "";
    }

    void
    Console::setDefaultErrorString()
    {
        m_errorCode = errorCode;
    }

    void Console::printFloat(float f, FloatPrecision p)
    {
        char res[20];
        EmbyLibs::ftoa(f, res, 4);
        print(res);
        print(" ");
    }

    bool
    Console::isTimeoutExpired()
    {
        if (BitMask_GET_MASK(m_flagMask, CONSOLE_FLAG_TIME_WAS_EXPIRED))
        {
            BitMask_CLR_MASK(m_flagMask, CONSOLE_FLAG_TIME_WAS_EXPIRED);
            return true;
        }
        return false;
    }

}
