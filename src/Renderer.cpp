#include "Renderer.h"

namespace LS{
	/*!
	  @brief   Sets the RI to be used to render the LEDs on the call to Render.
	  @param   riString		 The pointer to the string that contains the RI to be parsed.
	  @return  A boolean which is false if the RI is not valid or true if it is valid.
	*/
	bool Renderer::SetRI(const char* riString) {
		if (riString == nullptr) {
			return false;
		}

		const char* pRiString = riString;
		bool isValid;
		uint8_t ledIndex = 0;
		lastSetRiValid = false;

		while (*pRiString != '\0') {
			// get the RI
			RI ri = stringProcessor->ExtractRIFromHexEncoded(pRiString, isValid);
			if (isValid == false) return false;

			for (int i = 0; i < ri.number; i++) {
				pixelController->setPixelColor(ledIndex++, ri.colour.red, ri.colour.green, ri.colour.blue);

				if (ledIndex >= ledConfig->numberOfLEDs) {
					// all available LEDs have been set so we can return now
					lastSetRiValid = true;
					return true;
				}
			}

			// advance to the next RI
			pRiString += 8;				

			// peek the next character.  Is it an "R", meaning repeat from the beginning again?
			if (*pRiString != '\0' && *pRiString == 'R') {
				pRiString = riString;
			}
		}

		lastSetRiValid = ledIndex > 0;

		return lastSetRiValid;
	}

	/*!
	  @brief   Sets the instance that contains the configuration values of the LEDs.
	  @param   ledConfig	 The pointer to the LEDConfig instance that contains the configuration values.
	*/
	void Renderer::SetLEDConfig(const LEDConfig* ledConfig) {
		//if (ledConfig == nullptr) throw "ledConfig must not be a nullptr";
		//if (ledConfig->numberOfLEDs < 1) throw "ledConfig must specify a value for numberOfLEDs that is at least one";
		this->ledConfig = ledConfig;
	}

	/*!
	  @brief   Renders the LEDs (updates the LEDs) with the last RI that was set.  If an invalid RI
			   was set then the state of the LEDs will not be changed.
	*/
	void Renderer::Render() {
		if (lastSetRiValid) {
			pixelController->show();
		}
	}

	/*!
	  @brief	Checks whether any individual LEDs are currently turned on (that is, non-black).
	  @return	True if any single LED is currently on or false if all LEDs are off.
	*/
	bool Renderer::AreAnyLEDsOn() {
		for (uint8_t i = 0; i < ledConfig->numberOfLEDs; i++) {
			if (pixelController->getPixelColor(i) != 0) return true;
		}

		return false;
	}
}