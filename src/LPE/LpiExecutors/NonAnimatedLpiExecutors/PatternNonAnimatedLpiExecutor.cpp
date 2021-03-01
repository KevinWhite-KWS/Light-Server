#include "PatternNonAnimatedLpiExecutor.h"

namespace LS {
	/*!
		@brief		Valites that the LPI string is valid according to the
					rules for the pattern instruction.
		@returns	True if the pattern instruction is valid.
		@author		Kevin White
		@date		4 Jan 2021
	*/
	bool PatternNonAnimatedLpiExecutor::ValidateLpi(LpiExecutorParams* lpiExecParams)  {
		if (lpiExecParams == nullptr) {
			return false;
		}

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();
		StringProcessor* stringProcessor = lpiExecParams->GetStringProcesor();

		// we should have a number specifying the number of blocks in the pattern
		bool lpiIsValid = false;
		uint8_t numberOfBlocks = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 255, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// now we need to check that there are numberOfBlocks block width and colours
		// in the remaining lpiBuffer
		const char* blockWidthsBuffer = lpiBuffer + 2;
		const char* coloursBuffer = lpiBuffer + (2 * numberOfBlocks);
		for (uint8_t colourCounter = 0; colourCounter < numberOfBlocks; colourCounter++) {
			// get the width of the block which is a number between 1 and 255
			stringProcessor->ExtractNumberFromHexEncoded(blockWidthsBuffer, 1, 255, lpiIsValid);
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

		return true;
	}

	/*!
		@brief		Executes the solid instruction and populates the output.
		@param		lpiExecParams		The basic parametes necessary to execute an instruction.
		@param		output				A pointer to the class that is used to set the pixel
										outputs from executing the instruction.
		@author		Kevin White
		@date		2 Jan 2020
	*/
	void PatternNonAnimatedLpiExecutor::ExecuteNonAnimated(LpiExecutorParams* lpiExecParams, LpiExecutorOutput* output) {
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
		uint8_t numberOfBlocks = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 255, isValid);

		const char* coloursBuffer = blockWidthsBuffer + (2 * numberOfBlocks);
		// now, for numberOfBlocks add a rendering instruction for
		// the specified colour and number of pixel
		for (uint8_t blockCounter = 0; blockCounter < numberOfBlocks; blockCounter++) {
			uint8_t blockWidth = stringProcessor->ExtractNumberFromHexEncoded(blockWidthsBuffer, 1, 255, isValid);
			Colour blockColour = stringProcessor->ExtractColourFromHexEncoded(coloursBuffer, isValid);

			// add the rendering instruction for the colour and width
			output->SetNextRenderingInstruction(&blockColour, blockWidth);

			blockWidthsBuffer += 2;
			coloursBuffer += 6;
		}
		output->SetRepeatRenderingInstructions();
	}
}