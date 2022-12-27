#include "LoadProgramAndStoreCommand.h"

namespace LS {
	/*!
	  @brief   Executes the command that causes a Light
			   Program to be loaded.
	  @returns True if the command was executed successfully or
			   false if it did not execute successfully.
	*/
	bool LoadProgramAndStoreCommand::ExecuteCommand() {
		LoadProgramCommand::ExecuteCommand();
		
		// Persist the program in flash memory if required so the same program will be loaded next time
		// max program: 2000 chars or less
		if (strlen(lpBuffer->GetBuffer()) < 2000) {
			strcpy(ledConfig->storedProgram, lpBuffer->GetBuffer());
			configPersistance->SaveConfig(ledConfig);
		}

		return true;
	}
}