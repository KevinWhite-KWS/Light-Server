#include "RainbowAnimatedLpiExecutor.h"

namespace LS {
	/*!
			@brief		Valites that the LPI string is valid according to the
						rules for the rainbow instruction.
			@param		lpiExecParams		The basic parametes necessary to execute an instruction.
			@returns	True as the clear instruction is simple.
			@author		Kevin White
			@date		11 Jan 2021
	*/
	bool RainbowAnimatedLpiExecutor::ValidateLpi(LpiExecutorParams* lpiExecParams) {
		if (lpiExecParams == nullptr) {
			return false;
		}

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();
		StringProcessor* stringProcessor = lpiExecParams->GetStringProcesor();

		// first we expect a number that specifies the 'virtual' length of the effect
		bool lpiIsValid = false;
		stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 255, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// next we expect a number that specifies the step value of each animation step
		lpiBuffer += 2;
		stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 255, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// next, we should have a boolean for whether we start near side (0) or far side (1)
		lpiBuffer += 2;
		stringProcessor->ExtractBoolFromHexEncoded(lpiBuffer, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// now we expect a number that specifies the number of colours involved in the
		// rainbow
		lpiBuffer += 1;
		uint8_t numberOfColours = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 10, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// finally, we check that have numberOfColours colours remaining in the LPI buffer
		lpiBuffer += 2;
		for (int i = 0; i < numberOfColours; i++) {
			stringProcessor->ExtractColourFromHexEncoded(lpiBuffer, lpiIsValid);
			if (!lpiIsValid) {
				return false;
			}
			lpiBuffer += 6;
		}

		return true;
	}

	/*!
				@brief		Gets the number of animation steps to complete the rainbow
							effect for a specified LPI.
				@param		lpiExecParams		The basic parametes necessary to execute an instruction.
				@returns	The number of animation steps of the rainbow effect.
				@author		Kevin White
				@date		11 Jan 21
	*/
	uint16_t RainbowAnimatedLpiExecutor::GetNumberOfSteps(LpiExecutorParams* lpiExecParams) {
		if (lpiExecParams == nullptr) {
			return false;
		}

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();
		StringProcessor* stringProcessor = lpiExecParams->GetStringProcesor();

		// we need the step size and two colours in order to calculate the total steps
		bool isValid;
		uint8_t steps = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer + 2, 1, 50, isValid);

		return steps + 1;
	}

	/*!
		@brief		Executes the rainbow instruction and populates the output.
		@param		lpiExecParams		The basic parametes necessary to execute an instruction.
		@param		step				The step number in the animation which is to be rendered.
		@param		output				A pointer to the class that is used to set the pixel
										outputs from executing the instruction.
		@author		Kevin White
		@date		11 Jan 2020
	*/
	void RainbowAnimatedLpiExecutor::Execute(LpiExecutorParams* lpiExecParams, uint16_t step, LpiExecutorOutput* output) {
		if (lpiExecParams == nullptr
			|| output == nullptr) {
			return;
		}

		// ensure that specified step does not exceed the max number of steps
		uint16_t totalSteps = GetNumberOfSteps(lpiExecParams);
		if (step > totalSteps - 1) {
			return;
		}

		// reset the state of the output, including a flag that states that output has been set!
		output->Reset();

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();
		const char* colourBuffer = lpiBuffer + 7;
		StringProcessor* stringProcessor = lpiExecParams->GetStringProcesor();

		// get the parameters of the rainbow effect from the LPI
		bool isValid;
		uint8_t effectLength = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 255, isValid);
		uint8_t effectSteps = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer + 2, 1, 255, isValid);
		bool startFar = stringProcessor->ExtractBoolFromHexEncoded(lpiBuffer + 4, isValid);
		uint8_t numberOfColours = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer + 5, 1, 10, isValid);

		// calculate the value of each pixel for this step of the rainbow effect
		uint16_t ind = 0;
		uint8_t colourIndex1, colourIndex2;
		int switchVal = 0;
		float factor1, factor2 = 0;
		float numEffectStepsDivLength = effectSteps / effectLength;;
		float numEffectStepsDivNumColours = effectSteps / numberOfColours;
		bool colourIsValid;
		Colour rainbowPixelColour;

		for (uint16_t pixelIndex = 0; pixelIndex < lpiExecParams->GetLedConfig()->numberOfLEDs; pixelIndex++) {
			if (!startFar) {
				ind = step + pixelIndex * numEffectStepsDivLength;
			}
			else {
				ind = effectSteps - ((uint16_t)abs(step - pixelIndex * (uint16_t)numEffectStepsDivLength)) % effectSteps;
			}

			switchVal = (int)(ind % effectSteps) / numEffectStepsDivNumColours;
			factor1 = 1.0 - ((float)(ind % effectSteps - switchVal * numEffectStepsDivNumColours) / numEffectStepsDivNumColours);
			factor2 = (float)((int)(ind - (switchVal * numEffectStepsDivNumColours)) % effectSteps) / numEffectStepsDivNumColours;

			// get the colours from the buffer.  The first 7 bytes of the
			// instruction buffer contain the configuration properties.
			// The reamining 6x (x = number of colours) contain the colours.
			colourIndex1 = switchVal * 6;
			colourIndex2 = (switchVal == numberOfColours - 1 ? 0 : switchVal + 1) * 6;
			Colour colour1 = stringProcessor->ExtractColourFromHexEncoded(colourBuffer + colourIndex1, colourIsValid);
			Colour colour2 = stringProcessor->ExtractColourFromHexEncoded(colourBuffer + colourIndex2, colourIsValid);

			// now blend the colours and calculate the component r g b
			rainbowPixelColour.red = colour1.red * factor1 + colour2.red * factor2;
			rainbowPixelColour.green = colour1.green * factor1 + colour2.green * factor2;
			rainbowPixelColour.blue = colour1.blue * factor1 + colour2.blue * factor2;
			

			// add a rendering instruction for the pixel
			output->SetNextRenderingInstruction(&rainbowPixelColour, 1);
		}
	}
}