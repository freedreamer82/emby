#include "ConsoleCommands.hh"
#include <EmbyThreading/Thread.hh>

#include <EmbyTime/Clock.hh>

using namespace EmbyConsole;
using namespace EmbyLibs;
using namespace EmbyThreading;
using namespace EmbySystem;
using namespace EmbyMachine;


static const CommandInfo g_Commands[] =
        {
                {.cmd = "reboot", .userLevel = USER_ROOT, .cmdInfo = "reboot [delaySec] [bStayBootloader]"},
#if defined(configUSE_TRACE_FACILITY) && defined(configUSE_STATS_FORMATTING_FUNCTIONS)
                {.cmd = "ps", .userLevel = USER_ROOT, .cmdInfo = "show tasks"},
#endif
                {.cmd = "free", .userLevel = USER_GUEST, .cmdInfo = "heap status"},
                {.cmd = "info", .userLevel = USER_ROOT, .cmdInfo = "show info"},
        };




ConsoleCommandsProject::ConsoleCommandsProject(Context const &ctx)
        : m_ctx{ctx}
{
    m_tableCommands = g_Commands;
    m_tableCommandsSize = sizeof(g_Commands);
}

CommandErrorCode
ConsoleCommandsProject::cmdHandler_info(Console &console,
                                        char const *command,
                                        char const *const *args,
                                        int argsNo)
{
    (void) command;
    (void) args;
    (void) argsNo;

//    /*unique id is 80 bits 16(SIM_UIDMH) + 32(SIM_UIDML) +32(SIM_UIDL) */
//    console.printf("Fw version: %u.%u (%s),", FIRMWARE_VERSION_MAJOR,
//                   FIRMWARE_VERSION_MINOR,
//                   "DVC-Lite");
    console.printf("compiled@%s,%s\r\n", __DATE__, __TIME__);
    console.println("");


    return NO_ERR;
}


CommandErrorCode ConsoleCommandsProject::cmdHandler_date(Console &console,
                                                         char const *command,
                                                         char const *const *args,
                                                         int argsNo)
{
    EmbyTime::DateTime dateTime;
    EmbyTime::Clock::get().getTime(dateTime);
    EmbyTime::TimeStamp seconds;
    EmbyTime::DateTime::getTimeStamp(seconds, dateTime);

    console.printf("date UTC ts: %d ", seconds);
    console.printf("%d/%d/%d @ %d:%d:%s", dateTime.getDate().getDay(), dateTime.getDate().getMonth(),
                   dateTime.getDate().getYear(),
                   dateTime.getTime().getHours(), dateTime.getTime().getMinutes(), dateTime.getTime().getSeconds());

    return EmbyConsole::NO_ERR;

}


#if defined(configUSE_TRACE_FACILITY) && defined(configUSE_STATS_FORMATTING_FUNCTIONS)

CommandErrorCode
ConsoleCommandsProject::cmdHandler_ps(Console &console,
                                      char const *command,
                                      char const *const *args,
                                      int argsNo) {
    (void) command;
    (void) args;
    (void) argsNo;
#if(configUSE_TRACE_FACILITY == 1)
    char list[250];
    vTaskList(list);
    //Console_printf(console, "######         TASKS LIST         #######" EOL "" EOL);
    console.printf("|   NAME  |  STATE | PRIO | STK_SIZE | NUM\r\n\r\n");
    //console.printf(" __________________________________________\r\n");
    console.printf(list);
    return NO_ERR;
#else
    return CMD_NOT_FOUND;
#endif
}

#endif

CommandErrorCode
ConsoleCommandsProject::cmdHandler_free(Console &console,
                                        char const *command,
                                        char const *const *args,
                                        int argsNo)
{

#if(configUSE_TRACE_FACILITY == 1)
    (void) command;
    (void) args;
    (void) argsNo;

    uint32_t freeHeapSize = xPortGetFreeHeapSize();
    int used = configTOTAL_HEAP_SIZE - freeHeapSize;
    console.printf("Memory[bytes]: total %d,used %lu,free %d",
                   configTOTAL_HEAP_SIZE, used, freeHeapSize);
    console.println();
#endif

    return NO_ERR;
}


CommandErrorCode
ConsoleCommandsProject::handler(Console &console,
                                char const *command,
                                char const *const *args,
                                int argsNo)
{
    if (std::strcmp(command, "info") == 0)
    {
        return cmdHandler_info(console, command, args, argsNo);
    }

    if (std::strcmp(command, "date") == 0)
    {
        return cmdHandler_date(console, command, args, argsNo);
    }

    if (std::strcmp(command, "reboot") == 0)
    {
        return cmdHandler_reboot(console, command, args, argsNo);
    }

#if defined(configUSE_TRACE_FACILITY) && defined(configUSE_STATS_FORMATTING_FUNCTIONS)
    if (std::strcmp(command, "ps") == 0) {
        return cmdHandler_ps(console, command, args, argsNo);
    }
#endif

#if !defined(Emby_ARCH_LINUX_X86)
    if (std::strcmp(command, "free") == 0)
    {
        return cmdHandler_free(console, command, args, argsNo);
    }
#endif

    return CMD_NOT_FOUND;
}


CommandErrorCode
ConsoleCommandsProject::cmdHandler_reboot(Console &console,
                                          char const *command,
                                          char const *const *args,
                                          int argsNo)
{
    (void) command;
    (void) console;
    (void) args;
    int delaySec = 0;
    if (argsNo == 1)
    {
        delaySec = atoi(args[0]) * 1000;
    }
    EmbySystem::delayMs(delaySec);


    EmbySystem::reboot();

    return NO_ERR;
}

