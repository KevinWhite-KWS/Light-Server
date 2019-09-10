#include "PatternNonAnimatedLPI.h"

namespace LS {

	/*!
	  @brief   Fills the RI buffer with the next RI to be executed.  If there
			   is no RI to be executed next or the instruction is not in a valid
			   state then false is returned.
	  @returns True if the next RI buffer was correctly filled.  False otherwise.
	*/
	bool PatternNonAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer) {
		if (this->instructionIsValid == false) return false;

		// Ensure the buffer is wide enough
		uint16_t bufferSizeRequired = (this->numberOfBlocks * 8) + 2;
		if (riBuffer->GetBufferSize() < bufferSizeRequired) return false;

		// Fill the RI buffer with solid colour required
		char* pBlocksPointer = this->lpiInstruction->GetLPIBuffer() + 2;
		char* pColoursPointer = pBlocksPointer + (this->numberOfBlocks * 2);
		char* pRiBuffer = riBuffer->GetBuffer();

		for (int i = 0; i < this->numberOfBlocks; i++) {
			// put the colour into the buffer first
			for (int j = 0; j < 6; j++) *pRiBuffer++ = *pColoursPointer++;
			// put the number next
			for (int j = 0; j < 2; j++) *pRiBuffer++ = *pBlocksPointer++;
		}
		
		*pRiBuffer++ = 'R';
		*pRiBuffer = '\0';

		return true;
	}

	/*!
	  @brief   Validates the instruction according to the validation rules
			   for this type of LPI.
	  @returns True if the instruction is valid or false it is not.
	*/
	const bool PatternNonAnimatedLPI::ValidateInstruction() {
		char* pInstruction = this->lpiInstruction->GetLPIBuffer();

		// Ensure there's a number of blocks specified
		this->numberOfBlocks = this->stringProcessor->ExtractNumberFromHexEncoded(pInstruction, 1, 255, this->instructionIsValid);
		if (this->instructionIsValid == false) return false;
		pInstruction += 2;

		// Get the sum of the LEDs specified in the blocks.  We need to ensure this
		// is less than the total number of LEDs.
		uint16_t ledsSpecified = 0;
		for (int i = 0; i < this->numberOfBlocks; i++) {
			ledsSpecified += this->stringProcessor->ExtractNumberFromHexEncoded(pInstruction, 1, 255, this->instructionIsValid);
			if (this->instructionIsValid == false) return false;
			pInstruction += 2;
		}
		if (ledsSpecified > this->ledConfig->numberOfLEDs) {
			this->instructionIsValid = false;
			return false;
		}
		
		// Finally, ensure that we have a valid colour for each block
		for (int i = 0; i < this->numberOfBlocks; i++) {
			this->stringProcessor->ExtractColourFromHexEncoded(pInstruction, this->instructionIsValid);
			if (this->instructionIsValid == false) return false;
			pInstruction += 6;
		}

		return this->instructionIsValid;
	}
}


