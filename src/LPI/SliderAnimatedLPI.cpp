#include "SliderAnimatedLPI.h"

namespace LS {

	/*!
	  @brief   Fills the RI buffer with the next RI to be executed.  If there
			   is no RI to be executed next or the instruction is not in a valid
			   state then false is returned.
	  @returns True if the next RI buffer was correctly filled.  False otherwise.
	*/
	bool SliderAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer) {
		if (this->instructionIsValid == false) return false;

		// Ensure the buffer is wide enough
		if (riBuffer->GetBufferSize() < 10) return false;

		// Fill the RI buffer with solid colour required
		// const char* pInstruction = this->lpiInstruction->lpi->GetBuffer() + 8;
		char* pInstruction = this->lpiInstruction->GetLPIBuffer();
		char* pRiBuffer = riBuffer->GetBuffer();
		for (int i = 0; i < 6; i++) *pRiBuffer++ = *pInstruction++;
		
		// Now, add '01' (affect one pixel) and 'R' (repeat along the entire length).  Finally,
		// terminate the string.
		*pRiBuffer++ = '0';
		*pRiBuffer++ = '1';
		*pRiBuffer++ = 'R';
		*pRiBuffer = '\0';

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
		this->startNear = stringProcessor->ExtractBoolFromHexEncoded(pInstruction, this->instructionIsValid);
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

		return this->instructionIsValid;
	}
}


