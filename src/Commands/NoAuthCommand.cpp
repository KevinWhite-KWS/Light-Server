#include "NoAuthCommand.h"

namespace LS {
	/*!
	  @brief   Executes the command and causes the web connection
			   to be closed with a HTTP 401 response code.
	  @returns True if the command was executed successfully or
			   false if it did not execute successfully.
	*/
	bool NoAuthCommand::ExecuteCommand() {
		lightWebServer->RespondNotAuthorised();

		return true;
	}
}