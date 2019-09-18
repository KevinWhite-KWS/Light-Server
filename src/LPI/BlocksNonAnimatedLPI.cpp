#include "BlocksNonAnimatedLPI.h"

namespace LS {

	/*!
	  @brief   Fills the RI buffer with the next RI to be executed.  If there
			   is no RI to be executed next or the instruction is not in a valid
			   state then false is returned.
	  @returns True if the next RI buffer was correctly filled.  False otherwise.
	*/
	bool BlocksNonAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer) {
		if (this->instructionIsValid == false) return false;

		// Ensure the buffer is wide enough
		if (riBuffer->GetBufferSize() < (this->noOfBlocks * 8 +1)) return false;

		// Fill the RI buffer with solid colour required
		// const char* pInstruction = this->lpiInstruction->lpi->GetBuffer() + 8;
		char* pBlocks = this->lpiInstruction->GetLPIBuffer() + 2;
		char* pColours = pBlocks + (2 * this->noOfBlocks);
		char* pRiBuffer = riBuffer->GetBuffer();
		uint16_t penultimateBlock = this->noOfBlocks - 1;

		bool roundUp = true;

		uint16_t pixelsCovered = 0;
		for (int i = 0; i < this->noOfBlocks; i++) {
			// Get the block width
			uint8_t blockWidth = this->stringProcessor->ExtractNumberFromHexEncoded(pBlocks, 1, 100, this->instructionIsValid);
			pBlocks += 2;

			// Calculate the number of pixels to cover.  This is given as: blockWidth / 100 * noOfPixels
			double pixelsToCover = (double)blockWidth / 100 * this->ledConfig->numberOfLEDs;

			// Now, either round up or down depending on roundUp.  This alternatves per value which
			// kinda 'smooths' the effect.
			uint8_t pixels = 0;
			if (roundUp) {
				pixels = ceil(pixelsToCover);
			}
			else {
				pixels = floor(pixelsToCover);
			}

			// Write the target colour to the RI buffer
			for (int i = 0; i < 6; i++)* pRiBuffer++ = *pColours++;

			// Write the number of pixels to the RI buffer
			pixels = (i == penultimateBlock ? this->ledConfig->numberOfLEDs - pixelsCovered : pixels);
			this->stringProcessor->ConvertNumberToHexEncoded(pRiBuffer, pixels);
			pRiBuffer += 2;

			roundUp = !roundUp;
			pixelsCovered += pixels;
		}

		*pRiBuffer = '\0';

		return true;
	}

	/*!
	  @brief   Validates the instruction according to the validation rules
			   for this type of LPI.
	  @returns True if the instruction is valid or false it is not.
	*/
	const bool BlocksNonAnimatedLPI::ValidateInstruction() {
		char* pInstruction = this->lpiInstruction->GetLPIBuffer();

		// Get the number of blocks (between 2 - 10 is valid)
		this->noOfBlocks = this->stringProcessor->ExtractNumberFromHexEncoded(pInstruction, 2, 10, this->instructionIsValid);
		if (this->instructionIsValid == false) return false;
		pInstruction += 2;

		// Ensure there is noOfBlocks blocks and that the sum of those is 100
		uint8_t sumBlockWidths = 0;
		uint8_t currentBlockWidth = 0;
		char* pColours = pInstruction + (2 * this->noOfBlocks);
		for (int i = 0; i < this->noOfBlocks; i++) {
			currentBlockWidth = this->stringProcessor->ExtractNumberFromHexEncoded(pInstruction, 1, 100, this->instructionIsValid);
			if (this->instructionIsValid == false) return false;
			pInstruction += 2;
			sumBlockWidths += currentBlockWidth;

			// Now, extract the colour for the block (if possible)
			this->stringProcessor->ExtractColourFromHexEncoded(pColours, this->instructionIsValid);
			if (this->instructionIsValid == false) return false;
			pColours += 6;
		}
		if (sumBlockWidths != 100) return false;

		return this->instructionIsValid;
	}
}


