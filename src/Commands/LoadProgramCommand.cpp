#include "LoadProgramCommand.h"

namespace LS {
	/*!
	  @brief   Executes the command that causes a Light
			   Program to be loaded.
	  @returns True if the command was executed successfully or
			   false if it did not execute successfully.
	*/
	bool LoadProgramCommand::ExecuteCommand() {
		// Get the buffer from the request that contains the body
		// of the web request i.e. the Light Program that is to be loaded.
		//// char* buf = lightWebServer->GetLoadingBuffer(false);
		// FixedSizeCharBuffer* lpBuffer = lightWebServer->GetLoadingFixedSizeBuffer();
		lpBuffer = lightWebServer->GetLoadingFixedSizeBuffer();

		// Validate the Light Program
		lpValidator->ValidateLp(lpBuffer, &validateResult);

		if (validateResult.GetCode() != LS::LPValidateCode::Valid) {
			// The received Light Program is not validate.  Respond
			// with an error code 400.
			lightWebServer->RespondError();
			return false;
		}

		//// Persist the program in flash memory if required so the same program will be loaded next time
		//if (1 == 1 
		//	&& strlen(lpBuffer->GetBuffer()) < 2000) {
		//	Serial.println("Persisting program to flash");
		//	Serial.println(lpBuffer->GetBuffer());
		//	Serial.println("Persisted program size");
		//	Serial.println(strlen(lpBuffer->GetBuffer()));
		//	Serial.println("Free memory");
		//	Serial.println(freeMemory());

		//	// ledConfig->storedProgram = lpBuffer->GetBuffer();
		//	strcpy(ledConfig->storedProgram, lpBuffer->GetBuffer());
		//	// configPersistance->SaveConfig(ledConfig);
		//}

		// The LP is valid so we need to build a tree representation of
		// that Light Program which will be used to execute the program
		lpStateBuilder->BuildState(lpBuffer, lpState);
		
		// Finally, we can respond with a successful response.
		lightWebServer->RespondNoContent();

		return true;
	}
}