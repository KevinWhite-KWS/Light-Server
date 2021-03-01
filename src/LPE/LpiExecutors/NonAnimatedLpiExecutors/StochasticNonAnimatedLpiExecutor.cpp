#include "StochasticNonAnimatedLpiExecutor.h"

namespace LS {
	/*!
			@brief		Valites that the LPI string is valid according to the
						rules for the stochastic instruction.
			@returns	True if the stochastic instruction is valid.
			@author		Kevin White
			@date		8 Jan 2021
		*/
	bool StochasticNonAnimatedLpiExecutor::ValidateLpi(LpiExecutorParams* lpiExecParams) {
		if (lpiExecParams == nullptr) {
			return false;
		}

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();
		StringProcessor* stringProcessor = lpiExecParams->GetStringProcesor();

		// we should have a number specifying the number of colours
		bool lpiIsValid = false;
		uint8_t numberOfColours = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 2, 50, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// now we need to check that there are numberOfColours colours in the buffer
		lpiBuffer += 2;
		for (uint8_t colourCounter = 0; colourCounter < numberOfColours; colourCounter++) {
			stringProcessor->ExtractColourFromHexEncoded(lpiBuffer, lpiIsValid);
			if (!lpiIsValid) {
				return false;
			}
			lpiBuffer += 6;
		}

		return true;
	}

	/*!
			@brief		Executes the stochastic instruction and populates the output.
			@param		lpiExecParams		The basic parametes necessary to execute an instruction.
			@param		output				A pointer to the class that is used to set the pixel
											outputs from executing the instruction.
			@author		Kevin White
			@date		8 Jan 2020
	*/
	void StochasticNonAnimatedLpiExecutor::ExecuteNonAnimated(LpiExecutorParams* lpiExecParams, LpiExecutorOutput* output) {
		if (lpiExecParams == nullptr
			|| output == nullptr) {
			return;
		}

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();
		const char* coloursBuffer = lpiBuffer + 2;
		StringProcessor* stringProcessor = lpiExecParams->GetStringProcesor();

		// reset the state of the output, including a flag that states that output has been set!
		output->Reset();

		// get the number of blocks in the pattern first
		bool isValid = true;
		uint8_t numberOfColours = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 2, 50, isValid);

		// now, for each pixel pick, by random selection, one of the colours
		// that have been specified in the LPI
		Colour chosenRandomColour;
		for (uint16_t ledCounter = 0; ledCounter < lpiExecParams->GetLedConfig()->numberOfLEDs; ledCounter++) {
			uint8_t randColour = rand() % numberOfColours;

			chosenRandomColour = stringProcessor->ExtractColourFromHexEncoded(coloursBuffer + (6 * randColour), isValid);
			output->SetNextRenderingInstruction(&chosenRandomColour, 1);
		}
	}
}