#include "CommandFactory.h"

namespace LS {
	/*!
		@brief		Sets a reference to an individual command instance.  We set the references
					in this way as some of the constructors can be complex.
		@param		commandType		The type of command for the which the reference is being set.
		@param		command			A pointer to the command instance that is executed for the specified type.
		@author		Kevin White
		@date		11 Feb 21
	*/
	void CommandFactory::SetCommand(CommandType commandType, ICommand* command) {
		if (commandType == CommandType::NONE
			|| command == nullptr) {
			return;
		}

		switch (commandType) {
			case CommandType::NOAUTH:
				commands[0] = command;
				break;
			case CommandType::INVALID:
				commands[1] = command;
				break;
			case CommandType::LOADPROGRAM:
				commands[2] = command;
				break;
			case CommandType::LOADPROGRAMANDSTORE:
				commands[3] = command;
				break;
			case CommandType::POWEROFF:
				commands[4] = command;
				break;
			case CommandType::POWERON:
				commands[5] = command;
				break;
			case CommandType::CHECKPOWER:
				commands[6] = command;
				break;
			case CommandType::GETABOUT:
				commands[7] = command;
				break;
			case CommandType::SETLEDS:
				commands[8] = command;
				break;
		}
	}

	/*!
		@brief		Factory method gets a command instance
					for the specified command type.
		@param		commandType		The type of command to be returned.
		@returns	A pointer to the required command or nullptr if the command is not supported.
		@author		Kevin White
		@date		29 Jan 21
	*/
	ICommand* CommandFactory::GetCommand(CommandType commandType) {
		switch (commandType) {
			case CommandType::NOAUTH:
				return commands[0];
				break;
			case CommandType::INVALID:
				return commands[1];
				break;
			case CommandType::LOADPROGRAM:
				return commands[2];
				break;
			case CommandType::LOADPROGRAMANDSTORE:
				return commands[3];
				break;
			case CommandType::POWEROFF:
				return commands[4];
				break;
			case CommandType::POWERON:
				return commands[5];
				break;
			case CommandType::CHECKPOWER:
				return commands[6];
				break;
			case CommandType::GETABOUT:
				return commands[7];
				break;
			case CommandType::SETLEDS:
				return commands[8];
				break;
		}

		return nullptr;
	}
}