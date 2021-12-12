#include "BlocksNonAnimatedLpiExecutor.h"

namespace LS {
	/*!
		@brief		Valites that the LPI string is valid according to the
					rules for the blocks instruction.
		@returns	True if the pattern instruction is valid.
		@author		Kevin White
		@date		9 Jan 2021
	*/
	bool BlocksNonAnimatedLpiExecutor::ValidateLpi(LpiExecutorParams* lpiExecParams) {
		if (lpiExecParams == nullptr) {
			return false;
		}

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();
		StringProcessor* stringProcessor = lpiExecParams->GetStringProcesor();

		// we should have a number specifying the number of blocks
		bool lpiIsValid = false;
		uint8_t numberOfBlocks = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 2, 10, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// now we need to check that there are numberOfBlocks block width and colours
		// in the remaining lpiBuffer
		uint16_t totalWidth = 0;
		const char* blockWidthsBuffer = lpiBuffer + 2;
		const char* coloursBuffer = lpiBuffer + (2 * numberOfBlocks);
		for (uint8_t colourCounter = 0; colourCounter < numberOfBlocks; colourCounter++) {
			// get the width of the block which is a number between 1 and 255
			totalWidth += stringProcessor->ExtractNumberFromHexEncoded(blockWidthsBuffer, 1, 100, lpiIsValid);
			if (!lpiIsValid) {
				return false;
			}
			blockWidthsBuffer += 2;

			// get the colour of the block
			stringProcessor->ExtractColourFromHexEncoded(coloursBuffer, lpiIsValid);
			if (!lpiIsValid) {
				return false;
			}
			coloursBuffer += 6;
		}

		// total width must be 100 (that is, 100% of the available LEDs)
		if (totalWidth != 100) {
			return false;
		}

		return true;
	}

	/*!
			@brief		Executes the blocks instruction and populates the output.
			@param		lpiExecParams		The basic parametes necessary to execute an instruction.
			@param		output				A pointer to the class that is used to set the pixel
											outputs from executing the instruction.
			@author		Kevin White
			@date		9 Jan 2020
		*/
	void BlocksNonAnimatedLpiExecutor::ExecuteNonAnimated(LpiExecutorParams* lpiExecParams, LpiExecutorOutput* output) {
		if (lpiExecParams == nullptr
			|| output == nullptr) {
			return;
		}

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();
		const char* blockWidthsBuffer = lpiBuffer + 2;
		StringProcessor* stringProcessor = lpiExecParams->GetStringProcesor();

		// reset the state of the output, including a flag that states that output has been set!
		output->Reset();

		// get the number of blocks in the pattern first
		bool isValid = true;
		uint8_t numberOfBlocks = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 10, isValid);
		uint8_t penultimateBlock = numberOfBlocks - 1;

		const char* coloursBuffer = blockWidthsBuffer + (2 * numberOfBlocks);
		// now, for numberOfBlocks calculate the number of pixels
		// covered by that block and add a rendering instruction for the colour
		bool roundUp = true;
		uint16_t pixelsCovered = 0;
		for (uint8_t blockCounter = 0; blockCounter < numberOfBlocks; blockCounter++) {
			uint8_t blockWidth = stringProcessor->ExtractNumberFromHexEncoded(blockWidthsBuffer, 1, 100, isValid);
			Colour blockColour = stringProcessor->ExtractColourFromHexEncoded(coloursBuffer, isValid);

			// Calculate the number of pixels that the block width represents
			// as block width is a percent figure
			double pixelsToCover = (double)blockWidth / 100 * lpiExecParams->GetLedConfig()->numberOfLEDs;

			// Now, either round up or down depending on roundUp.  This alternatves per value which
			// kinda 'smooths' the effect.
			uint16_t pixels = 0;
			if (roundUp) {
				pixels = ceil(pixelsToCover);
			}
			else {
				pixels = floor(pixelsToCover);
			};

			pixels = (blockCounter == penultimateBlock ? lpiExecParams->GetLedConfig()->numberOfLEDs - pixelsCovered : pixels);

			// add the rendering instruction for the colour and width
			output->SetNextRenderingInstruction(&blockColour, pixels);

			blockWidthsBuffer += 2;
			coloursBuffer += 6;
			roundUp = !roundUp;
			pixelsCovered += pixels;
		}
	}
}