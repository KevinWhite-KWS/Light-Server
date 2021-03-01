#include "PowerOffCommand.h"

namespace LS {
	/*!
	  @brief   Executes the command that causes the power
			   to be turned-off to the LEDs.
	  @returns True if the command was executed successfully or
			   false if it did not execute successfully.
	*/
	bool PowerOffCommand::ExecuteCommand() {
		// Stop any lgiht program that is currently executing or otherwise
		// the next instruction will be rendered immediatly after the LEDS
		// have been 'turned-off'
		orchastor->StopPrograms();

		// Set the LEDs to black - effectively turning them off
		pixelController->fill(0);
		pixelController->show();

		// Respond with a 204 - no content reponse
		lightWebServer->RespondNoContent();

		return true;
	}
}