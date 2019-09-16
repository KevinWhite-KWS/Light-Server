#include "StochasticNonAnimatedLPI.h"

namespace LS {

	/*!
	  @brief   Fills the RI buffer with the next RI to be executed.  If there
			   is no RI to be executed next or the instruction is not in a valid
			   state then false is returned.
	  @returns True if the next RI buffer was correctly filled.  False otherwise.
	*/
	bool StochasticNonAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer) {
		if (this->instructionIsValid == false) return false;

		// Ensure the buffer is wide enough
		int maxBuffer = this->noOfElements * 8;
		if (riBuffer->GetBufferSize() < maxBuffer + 1) return false;

		// Iterate until the buffer has been filled with random colours from
		// the specified colours
		char* pInstruction = this->lpiInstruction->GetLPIBuffer() + 2;
		char* pRiBuffer = riBuffer->GetBuffer();
		uint8_t pixelsFilled = 0;
		uint8_t penultimatePixel = this->ledConfig->numberOfLEDs - 1;

		while (pixelsFilled < this->ledConfig->numberOfLEDs) {
			uint8_t randColour = rand() % this->noOfElements;
			uint8_t randPixels = (pixelsFilled == penultimatePixel ? 1 : rand() % 2 + 1);

			// Add the random colour to the buffer
			char* randomColour = pInstruction + (randColour * 6);
			for (int i = 0; i < 6; i++)* pRiBuffer++ = *randomColour++;

			// Add the number to the RI
			this->stringProcessor->ConvertNumberToHexEncoded(pRiBuffer, randPixels);
			pRiBuffer += 2;

			pixelsFilled += randPixels;
		}
		
		*pRiBuffer = '\0';

		return true;
	}

	/*!
	  @brief   Validates the instruction according to the validation rules
			   for this type of LPI.
	  @returns True if the instruction is valid or false it is not.
	*/
	const bool StochasticNonAnimatedLPI::ValidateInstruction() {
		char* pInstruction = this->lpiInstruction->GetLPIBuffer();

		// Extract the number of elements - the specification of the number of colours involved
		this->noOfElements = this->stringProcessor->ExtractNumberFromHexEncoded(pInstruction, 2, 50, this->instructionIsValid);
		if (this->instructionIsValid == false) return false;
		pInstruction += 2;

		// Ensure that there are numberOfElements valid colours in the instruction
		for (int i = 0; i < this->noOfElements; i++) {
			this->stringProcessor->ExtractColourFromHexEncoded(pInstruction, this->instructionIsValid);
			if (this->instructionIsValid == false) return false;
			pInstruction += 6;
		}

		return this->instructionIsValid;
	}
}


