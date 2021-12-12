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
		uint32_t newNoLeds = stringProcessor->ExtractNumberFromHex(buf, 10, 350, validNumber);

		// newNoLeds is a hex encoded value!  Dec encoded better?
		// is it consistent with the rest of the API?
		if (!validNumber) {
			lightWebServer->RespondError();

			return false;
		}
		
		// clear all LEDs that are on at the moment and set the new length of pixels
		pixelController->fill(0, 0, 0);
		pixelController->show();
		pixelController->updateLength((uint16_t)newNoLeds);

		// save the new number of LEDs to config persistent storage
		ledConfig->numberOfLEDs = newNoLeds;
		configPersistance->SaveConfig(ledConfig);

		// Reset the program state as it results in strange renderning behaviour
		// i.e. user has to upload a new program after configuring the LEDs
		programState->reset();

		// TODO: Re-config the LEDs
		lightWebServer->RespondNoContent();

		return true;
	}
}