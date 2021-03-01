#include "GetAboutCommand.h"

namespace LS {
	/*!
	  @brief   Executes the command that causes that
			   gets information about the server.
	  @returns True if the command was executed successfully or
			   false if it did not execute successfully.
	*/
	bool GetAboutCommand::ExecuteCommand() {
		webDoc->clear();
		(*webDoc)["LEDs"] = ledConfig->numberOfLEDs;
		(*webDoc)["LS Version"] = LS_VERSION;
		(*webDoc)["LDL Version"] = LDL_VERSION;
		serializeJsonPretty(*webDoc, webResponse->GetBuffer(), 1000);

		lightWebServer->RespondOK(webResponse->GetBuffer());
		
		return true;
	}
}