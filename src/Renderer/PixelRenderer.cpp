#include "PixelRenderer.h"

namespace LS {
	/*!
		@brief		Constructor injects dependencies.
		@param		pixelController		A pointer to the class that interacts with the LED hardware.
		@param		ledConfig			A pointer to the class that contains configuration information about the LEDs.
		@author		Kevin White
		@date		16 Jan 21
	*/
	PixelRenderer::PixelRenderer(IPixelController* pixelController, LEDConfig* ledConfig) {
		this->pixelController = pixelController;
		this->ledConfig = ledConfig;
	}

	/*!
	  @brief	Sets the pixel rendering buffer with the values that have been output
				from executing a rendering instruction.
	  @param	lpiExecutorOutput	A pointer to the instance that contains the rendering instruction output.
	  @return	True if the pixel rendering buffer was set or false if the lpiExecutorOutput is null
				or contains no rendering instructions.
	  @author	Kevin White
	  @date		16 Jan 21
	*/
	bool PixelRenderer::SetPixels(LpiExecutorOutput* lpiExecutorOutput) {
		lastSetRiValid = false;

		if (lpiExecutorOutput == nullptr
			|| lpiExecutorOutput->GetNumberOfRenderingInstructions() <= 0) {
			return false;
		}

		RI* renderingInstructions = lpiExecutorOutput->GetRenderingInstructions();
		uint16_t ledIndex = 0;
		uint16_t numberOfLeds = ledConfig->numberOfLEDs;
		lastSetRiValid = true;

		while (ledIndex < numberOfLeds) {
			// iterate over each RI and render the specified number of LEDs
			// for that RI
			for (uint16_t riIndex = 0; riIndex < lpiExecutorOutput->GetNumberOfRenderingInstructions(); riIndex++) {
				RI renderingInstruction = renderingInstructions[riIndex];

				for (uint16_t i = 0; i < renderingInstruction.number; i++) {
					pixelController->setPixelColor(
						ledIndex++,
						renderingInstruction.colour.red,
						renderingInstruction.colour.green,
						renderingInstruction.colour.blue
					);

					if (ledIndex >= numberOfLeds) {
						return true;
					}
				}
			}

			if (!lpiExecutorOutput->GetRepeatRenderingInstructions()) {
				// RIs do not repeat, so they have been rendered once
				// and now we just need to return
				return true;
			}
		}

		return true;
	}

	/*!
	  @brief	Renders the LEDs (updates the LEDs) with the last RI that was set.  If an invalid RI
				was set then the state of the LEDs will not be changed.
	  @author	Kevin White
	  @date		16 Jan 21
	*/
	void PixelRenderer::ShowPixels() {
		if (lastSetRiValid) {
			pixelController->show();
		}
	}

	/*!
		@brief		Checks whether any individual LEDs are currently turned on (that is, non-black).
		@returns	True if any LED is turned on or false if all are turned off.
		@author		Kevin White
		@date		16 Jan 21
	*/
	bool PixelRenderer::AreAnyPixelsOn() {
		for (uint16_t i = 0; i < ledConfig->numberOfLEDs; i++) {
			if (pixelController->getPixelColor(i) != 0) {
				return true;
			}
		}

		return false;
	}
}