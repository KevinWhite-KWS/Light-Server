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
			newR = startColour.red + currentStep * step;
			newG = startColour.green + currentStep * step;
			newB = startColour.blue + currentStep * step;

			newR = min(newR, this->endColour.red);
			newG = min(newG, this->endColour.green);
			newB = min(newB, this->endColour.blue);
		}
		else {
			int r = endColour.red - currentStep * step;
			int g = endColour.green - currentStep * step;
			int b = endColour.blue - currentStep * step;

			//newR = max(r, this->endColour.red);
			//newG = max(g, this->endColour.green);
			//newB = max(b, this->endColour.blue);
			newR = max(r, this->startColour.red);
			newG = max(g, this->startColour.green);
			newB = max(b, this->startColour.blue);
		}

		long val = this->endColour.green - this->currentStep * this->step;

		// Write the colours to the output RI buffer
		char* pRiBuffer = riBuffer->GetBuffer();
		this->stringProcessor->ConvertNumberToHexEncoded(pRiBuffer, newR);
		this->stringProcessor->ConvertNumberToHexEncoded(pRiBuffer += 2, newG);
		this->stringProcessor->ConvertNumberToHexEncoded(pRiBuffer += 2, newB);
		this->stringProcessor->ConvertNumberToHexEncoded(pRiBuffer += 2, this->ledConfig->numberOfLEDs);

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
		int redDiff = endColour.red - startColour.red;
		int greenDiff = endColour.green - startColour.green;
		int blueDiff = endColour.blue - startColour.blue;

		// int maxDiff = fmax(this->endColour.red - this->startColour.red, this->endColour.green - this->endColour.green);
		// maxDiff = fmax(maxDiff, this->endColour.blue - this->startColour.blue);
		int maxDiff = max(redDiff, greenDiff);
		maxDiff = max(maxDiff, blueDiff);
		double steps = (double)maxDiff / (double)step;
		if (steps < 0) steps = 0;
		this->totalSteps = ceil(steps) + 1;		// rounding!!!

		// Reset the current step to 0
		currentStep = 0;

		return this->instructionIsValid;
	}
}


