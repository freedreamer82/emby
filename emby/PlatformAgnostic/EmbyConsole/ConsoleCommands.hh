#if ! defined (EMBY_CONSOLE_COMMANDS)
#define EMBY_CONSOLE_COMMANDS

#include <cstddef>

namespace EmbyConsole
{
	class Console;


	typedef enum
	{
		USER_NONE = 0,
		USER_GUEST ,
		USER_ROOT
	} User;

	typedef enum
	{
		NO_ERR,
		CMD_NOT_FOUND,
		BAD_FORMAT
	}
	CommandErrorCode;

	typedef struct CommandErrorElem
	{
		CommandErrorCode  code ;
		const char * str;
	}
	CommandErrorElem;

	typedef struct
	{
		char const* cmd;
		User        userLevel;
		char const* cmdInfo;
	}
	CommandInfo;

	/**
	 *	Abstract Class for ConsoleCommands
	 *	*/
	class ConsoleCommands
	{
		public:
			ConsoleCommands( ConsoleCommands const& ) = delete;
			ConsoleCommands& operator=( ConsoleCommands const& ) = delete;

		    CommandErrorCode
			doCommand( Console & console,
			           char const* command,
			           char const* const* args,
			           int argsNo )
			{
				return handler( console , command ,args , argsNo);
				/*
				 * return Command::process( console , command ,args , argsNo);
				 */
			}

			CommandInfo const *
			getCommandsDescription() const
			{
				return m_tableCommands;
			}

			size_t
			getCommandsDescriptionSize() const
			{
				return m_tableCommandsSize;
			}

		protected:
			ConsoleCommands() : m_tableCommands(nullptr) , m_tableCommandsSize(0)
			{}

			virtual ~ConsoleCommands();

			virtual CommandErrorCode
			handler( Console& console,
	                 char const* command,
	                 char const* const* args,
	                 int argsNo ) = 0;

			CommandInfo const * m_tableCommands;
			size_t				m_tableCommandsSize;
	};
}

#endif
