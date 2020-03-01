#include "CheckPowerCommand.h"

namespace LS {
	/*!
	  @brief   Executes the command that checks the
			   power status of the LEDs.
	  @returns True if the command was executed successfully or
			   false if it did not execute successfully.
	*/
	bool CheckPowerCommand::ExecuteCommand() {
		// Check whether any LED is on (i.e. a non-zero value)
		uint16_t pixelCounter = 0;
		uint16_t totalPixels = pixelController->numPixels();
		bool anyOn = false;
		while (!anyOn && pixelCounter < totalPixels) {
			anyOn = pixelController->getPixelColor(pixelCounter++) != 0;
		}

		// Respond with the body response - i.e. state of pixels
		webDoc->clear();
		(*webDoc)["power"] = (anyOn ? "on" : "off");
		serializeJsonPretty(*webDoc, webResponse->GetBuffer(), 1000);

		// Send a 200 reponse back with the state of the LEDs in the body
		lightWebServer->RespondOK(webResponse->GetBuffer());

		return true;
	}
}