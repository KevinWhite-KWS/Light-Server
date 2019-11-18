#include "SliderAnimatedLPI.h"

namespace LS {

	/*!
	  @brief   Fills the RI buffer with the next RI to be executed.  If there
			   is no RI to be executed next or the instruction is not in a valid
			   state then false is returned.
	  @returns True if the next RI buffer was correctly filled.  False otherwise.
	*/
	bool SliderAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer) {
		if (this->instructionIsValid == false
			|| riBuffer->GetBufferSize() < 25			// maximum buffer size required for any step
			|| this->currentStep == this->totalSteps) return false;

		// Calculate the number of LEDs to be lite before the slider with the background colour
		uint8_t preSliderLEDs = this->currentStep;

		// Calculate the number of LEDS to be lite after the lisder with the background colour
		uint8_t postSliderLEDS = ledConfig->numberOfLEDs - this->sliderWidth - preSliderLEDs;

		// The pre and post background LED figures need inverting if the slider
		// is to start at the far side
		if (this->startFar == true) {
			uint8_t pre = preSliderLEDs;
			preSliderLEDs = postSliderLEDS;
			postSliderLEDS = pre;
		}

		// Fill the RI buffer with the background IF there are pre slider LEDs
		char* pInstruction = this->lpiInstruction->GetLPIBuffer();
		char* ri = riBuffer->GetBuffer();
		if (preSliderLEDs > 0) {
			for (int i = 0; i < 6; i++) *ri++ = *(pInstruction + 9 + i);
			this->stringProcessor->ConvertNumberToHexEncoded(ri, preSliderLEDs);
			ri += 2;
		}

		// Fill the RI buffer with the colour of the slider
		for (int i = 0; i < 6; i++) *ri++ = *(pInstruction + 3 + i);
		this->stringProcessor->ConvertNumberToHexEncoded(ri, this->sliderWidth);
		ri += 2;

		// Fill the RI buffer with the background IF there are post slider LEDs
		if (postSliderLEDS > 0) {
			for (int i = 0; i < 6; i++) *ri++ = *(pInstruction + 9 + i);
			this->stringProcessor->ConvertNumberToHexEncoded(ri, postSliderLEDS);
			ri += 2;
		}

		// increatment step for next animation frame
		this->currentStep++;

		return true;
	}

	/*!
	  @brief   Validates the instruction according to the validation rules
			   for this type of LPI.
	  @returns True if the instruction is valid or false it is not.
	*/
	const bool SliderAnimatedLPI::ValidateInstruction() {
		char* pInstruction = this->lpiInstruction->GetLPIBuffer();

		// Attempt to get a valid width (first two hex characters)
		// NOTE: max width of slider is noLEDS / 2
		int maxSliderWidth = ledConfig->numberOfLEDs / 2;
		this->sliderWidth = stringProcessor->ExtractNumberFromHexEncoded(pInstruction, 1, maxSliderWidth, this->instructionIsValid);
		if (this->instructionIsValid == false) return false;

		// Now, get the starting position of the slider which is either 0 (near)
		// or 1 (far)
		pInstruction += 2;
		this->startFar = stringProcessor->ExtractBoolFromHexEncoded(pInstruction, this->instructionIsValid);
		if (this->instructionIsValid == false) return false;

		// Ensure the slider colour is valid
		pInstruction += 1;
		stringProcessor->ExtractColourFromHexEncoded(pInstruction, this->instructionIsValid);
		if (this->instructionIsValid == false) return false;

		// Ensure the background colour is valid
		pInstruction += 6;
		stringProcessor->ExtractColourFromHexEncoded(pInstruction, this->instructionIsValid);
		if (this->instructionIsValid == false) return false;

		// Finally, everything is good.  Now calculate the total number of steps
		// involved in the animation effect.
		this->totalSteps = this->ledConfig->numberOfLEDs - this->sliderWidth + 1;

		// Ensure we reset the current step!
		currentStep = 0;

		return this->instructionIsValid;
	}
}


