#include "SetLedsCommand.h"

namespace LS {
	/*!
	  @brief   Executes the command to set the configuration of the connected leds.
	  @returns True if the command was executed successfully or
			   false if it did not execute successfully.
	*/
	bool SetLedsCommand::ExecuteCommand() {
		char* buf = lightWebServer->GetLoadingBuffer(false);

		bool validNumber = false;
		int newNoLeds = stringProcessor->ExtractNumberFromHexEncoded(buf, 10, 200, validNumber);
		// newNoLeds is a hex encoded value!  Dec encoded better?
		// is it consistent with the rest of the API?

		if (!validNumber) {
			lightWebServer->RespondError();

			return false;
		}
		
		// clear all LEDs that are on at the moment and set the new length of pixels
		pixelController->fill(0, 0, 0);
		pixelController->show();
		pixelController->updateLength(newNoLeds);
		
		// save the new number of LEDs to config persistent storage
		ledConfig->numberOfLEDs = newNoLeds;
		configPersistance->SaveConfig(ledConfig);

		// TODO: Re-config the LEDs
		lightWebServer->RespondNoContent();

		return true;
	}
}