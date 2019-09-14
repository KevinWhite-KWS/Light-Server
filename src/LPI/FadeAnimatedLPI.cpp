#include "FadeAnimatedLPI.h"

namespace LS {

	/*!
	  @brief   Fills the RI buffer with the next RI to be executed.  If there
			   is no RI to be executed next or the instruction is not in a valid
			   state then false is returned.
	  @returns True if the next RI buffer was correctly filled.  False otherwise.
	*/
	bool FadeAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer) {
		if (this->instructionIsValid == false
			|| riBuffer->GetBufferSize() < 10			// maximum buffer size required for any step
			|| this->currentStep == this->totalSteps) return false;

		// Calculate new values for RGB
		int newR = 0, newG = 0, newB = 0;

		if (this->fadeOut == false) {
			newR = this->startColour.red + this->currentStep * this->step;
			newR = min(newR, this->endColour.red);
			newR = min(newR, 255);
			newG = this->startColour.green + this->currentStep * this->step;
			newG = min(newG, this->endColour.green);
			newG = min(newG, 255);
			newB = this->startColour.blue + this->currentStep * this->step;
			newB = min(newB, this->endColour.blue);
			newB = min(newB, 255);
		}
		else {
			newR = this->endColour.red - this->currentStep * this->step;
			newR = max(newR, this->startColour.red);
			newR = max(newR, 0);
			newG = this->endColour.green - this->currentStep * this->step;
			newG = max(newG, this->startColour.green);
			newG = max(newG, 0);
			newB = this->endColour.blue - this->currentStep * this->step;
			newB = max(newB, this->startColour.blue);
			newB = max(newB, 0);
		}

		// Write the colours to the output RI buffer
		char* pRiBuffer = riBuffer->GetBuffer();
		this->stringProcessor->ConvertNumberToHexEncoded(pRiBuffer, newR);
		pRiBuffer += 2;
		this->stringProcessor->ConvertNumberToHexEncoded(pRiBuffer, newG);
		pRiBuffer += 2;
		this->stringProcessor->ConvertNumberToHexEncoded(pRiBuffer, newB);
		pRiBuffer += 2;
		this->stringProcessor->ConvertNumberToHexEncoded(pRiBuffer, this->ledConfig->numberOfLEDs);

		// increatment step for next animation frame
		this->currentStep++;

		return true;
	}

	/*!
	  @brief   Validates the instruction according to the validation rules
			   for this type of LPI.
	  @returns True if the instruction is valid or false it is not.
	*/
	const bool FadeAnimatedLPI::ValidateInstruction() {
		char* pInstruction = this->lpiInstruction->GetLPIBuffer();

		// Get the step size (between 1 - 50)
		this->step = this->stringProcessor->ExtractNumberFromHexEncoded(pInstruction, 1, 50, this->instructionIsValid);
		if (this->instructionIsValid == false) return false;
		pInstruction += 2;

		// Get whether we are fading in (0) or fading out (1)
		this->fadeOut = this->stringProcessor->ExtractBoolFromHexEncoded(pInstruction, this->instructionIsValid);
		if (this->instructionIsValid == false) return false;
		pInstruction += 1;

		// Get the start colour
		this->startColour = this->stringProcessor->ExtractColourFromHexEncoded(pInstruction, this->instructionIsValid);
		if (this->instructionIsValid == false) return false;
		pInstruction += 6;

		// Get the end colour
		this->endColour = this->stringProcessor->ExtractColourFromHexEncoded(pInstruction, this->instructionIsValid);
		if (this->instructionIsValid == false) return false;

		// if fade out then simply switch the end / start colours
		if (this->fadeOut == true) {
			Colour tempColour = this->startColour;
			this->startColour = this->endColour;
			this->endColour = tempColour;
		}

		// Calculate the number of steps
		int maxDiff = max(this->endColour.red - this->startColour.red, this->endColour.green - this->endColour.blue);
		maxDiff = max(maxDiff, this->endColour.blue - this->startColour.blue);
		double steps = (double)maxDiff / (double)step;
		this->totalSteps = ceil(steps) + 1;		// rounding!!!

		return this->instructionIsValid;
	}
}


