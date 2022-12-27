#ifndef _CommandFactory_h
#define _CommandFactory_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\WProgram.h"
#endif

#include "ICommand.h"
#include "CheckPowerCommand.h"
#include "GetAboutCommand.h"
#include "InvalidCommand.h"
#include "LoadProgramCommand.h"
#include "NoAuthCommand.h"
#include "PowerOffCommand.h"
#include "PowerOnCommand.h"
#include "SetLedsCommand.h"

namespace LS {
	/*!
		@brief	Factory class gets individual command instances.
		@author	Kevin White
		@date	19 Jan 2021
	*/
	class CommandFactory {
	private:
		ICommand* commands[9];

	public:
		virtual void SetCommand(CommandType commandType, ICommand* command);

		virtual ICommand* GetCommand(CommandType commandType);
	};
}


#endif