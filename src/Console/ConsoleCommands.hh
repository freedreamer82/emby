#ifndef CONSOLE_COMMANDS_HH
#define CONSOLE_COMMANDS_HH

#include <EmbyConsole/Console.hh>

#define CONSOLE_CMD_HANDLER(name)                    \
    EmbyConsole::CommandErrorCode cmdHandler_##name( \
        EmbyConsole::Console& console, char const* command, char const* const* args, int argsNo)

class ConsoleCommandsProject : public EmbyConsole::ConsoleCommands
{
public:
    struct Context
    {


    };

    ConsoleCommandsProject(Context const &ctx);

    Context *getContext()
    {
        return &m_ctx;
    }

private:
    Context m_ctx;


    CONSOLE_CMD_HANDLER(info);

    CONSOLE_CMD_HANDLER(reboot);

    CONSOLE_CMD_HANDLER(date);

    CONSOLE_CMD_HANDLER(free);

    CONSOLE_CMD_HANDLER(ps);

    EmbyConsole::CommandErrorCode handler(EmbyConsole::Console &console,
                                          char const *command,
                                          char const *const *args,
                                          int argsNo) override;
};

#endif // CONSOLE_COMMANDS_HH
