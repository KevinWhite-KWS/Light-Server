#include "PowerOnCommand.h"

namespace LS {
	/*!
	  @brief   Executes the command that causes the power
			   to be turned-on to the LEDs.
	  @returns True if the command was executed successfully or
			   false if it did not execute successfully.
	*/
	bool PowerOnCommand::ExecuteCommand() {
		// Stop any lgiht program that is currently executing or otherwise
		// the next instruction will be rendered immediatly after the LEDS
		// have been 'turned-off'
		orchastor->StopPrograms();

		// Set the LEDs to black - effectively turning them off
		pixelController->fill(0);
		pixelController->show();

		// See if we received a valid colour
		char* buf = lightWebServer->GetLoadingBuffer(false);
		bool isValidColour = false;
		Colour colour = stringProcessor->ExtractColourFromHexEncoded(buf, isValidColour);

		if (isValidColour) {
			// Use that valid colour
			uint32_t packedColour = PackColor(colour.red, colour.green, colour.blue);
			pixelController->fill(packedColour);
		}
		else {
			// Otherwise just fill them white
			pixelController->fill(16777215);
		}
		pixelController->show();

		// Respond with a 204 - no content reponse
		lightWebServer->RespondNoContent();

		return true;
	}
}