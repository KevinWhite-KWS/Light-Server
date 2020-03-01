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
		char* buf = lightWebServer->GetLoadingBuffer(false);

		// Load the request body into the Light Program buffer (lpBuffer)
		// so that it can be used to validate the program and
		// load the instruction.
		lpBuffer->ClearBuffer();
		lpBuffer->LoadFromBuffer(buf);

		// Validate the Light Program
		LPValidateResult result = LPValidateResult();
		lpe->ValidateLP(lpBuffer, &result);

		if (result.GetCode() != LS::LPValidateCode::Valid) {
			// The received Light Program is not validate.  Respond
			// with an error code 400.
			lightWebServer->RespondError();
			return false;
		}
		else {
			// The received Light Program is validate.  Respond with
			// a success code of 204.
			lightWebServer->RespondNoContent();
		}

		return true;
	}
}