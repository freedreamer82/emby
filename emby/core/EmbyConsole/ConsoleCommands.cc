
#include <EmbyConsole/ConsoleCommands.hh>

namespace EmbyConsole
{

    ConsoleCommands::~ConsoleCommands()
    {
    }

    ConsoleCommands::ConsoleCommands(CommandInfo const *table, size_t entries) :
            m_tableCommands{table},
            m_tableCommandsCount{entries}
    {
    }


}  // end of namespace EmbyConsole
///@}


