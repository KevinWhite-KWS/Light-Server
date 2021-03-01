#include "SliderAnimatedLpiExecutor.h"

namespace LS {
	/*!
		@brief		Valites that the LPI string is valid according to the
					rules for the slider instruction.
		@param		lpiExecParams		The basic parametes necessary to execute an instruction.
		@returns	True as the clear instruction is simple.
		@author		Kevin White
		@date		4 Jan 2021
	*/
	bool SliderAnimatedLpiExecutor::ValidateLpi(LpiExecutorParams* lpiExecParams) {
		if (lpiExecParams == nullptr) {
			return false;
		}

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();
		StringProcessor* stringProcessor = lpiExecParams->GetStringProcesor();

		// we should have a number specifying the width of the slider
		bool lpiIsValid = false;
		uint8_t numberOfBlocks = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 50, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// next, we should have a boolean for whether we need to start near (0) or far (1)
		lpiBuffer += 2;
		bool startNear = stringProcessor->ExtractBoolFromHexEncoded(lpiBuffer, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// finally, we should have two colours - first one for slider and second for background
		lpiBuffer += 1;
		for (int i = 0; i < 2; i++) {
			stringProcessor->ExtractColourFromHexEncoded(lpiBuffer, lpiIsValid);
			if (!lpiIsValid) {
				return false;
			}
			lpiBuffer += 6;
		}

		return true;
	}

	/*!
		@brief		Gets the number of animation steps to complete the slider
					effect for a specified slider LPI.
		@param		lpiExecParams		The basic parametes necessary to execute an instruction.
		@returns	The number of animation steps of the slider effect.
		@
	*/
	uint16_t SliderAnimatedLpiExecutor::GetNumberOfSteps(LpiExecutorParams* lpiExecParams) {
		if (lpiExecParams == nullptr) {
			return false;
		}

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();
		StringProcessor* stringProcessor = lpiExecParams->GetStringProcesor();

		// we need the slider width in order to calculate the number of steps
		bool isValid;
		uint8_t sliderWidth = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 255, isValid);

		// now we can calculate the total steps involved
		uint16_t totalSteps = lpiExecParams->GetLedConfig()->numberOfLEDs - sliderWidth + 1;

		return totalSteps;
	}

	/*!
		@brief		Executes the slider instruction and populates the output.
		@param		lpiExecParams		The basic parametes necessary to execute an instruction.
		@param		step				The step number in the animation which is to be rendered.  For example:
										if the slider has 5 animation steps and step is 2 then the 2nd
										animation step will be rendered.
		@param		output				A pointer to the class that is used to set the pixel
										outputs from executing the instruction.
		@author		Kevin White
		@date		4 Jan 2020
	*/
	void SliderAnimatedLpiExecutor::Execute(LpiExecutorParams* lpiExecParams, uint16_t step, LpiExecutorOutput* output) {
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
		StringProcessor* stringProcessor = lpiExecParams->GetStringProcesor();

		// get the parameters of the slider from the LPI
		bool isValid;
		uint8_t sliderWidth = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 50, isValid);
		bool startFar = stringProcessor->ExtractBoolFromHexEncoded(lpiBuffer + 2, isValid);
		Colour sliderColour = stringProcessor->ExtractColourFromHexEncoded(lpiBuffer + 3, isValid);
		Colour backgroundColour = stringProcessor->ExtractColourFromHexEncoded(lpiBuffer + 9, isValid);

		// calculate the values that determine the number of LEDs to be rendered
		uint8_t numLedsBeforeSlider = step;
		uint8_t numLedsAfterSlider = lpiExecParams->GetLedConfig()->numberOfLEDs - sliderWidth - numLedsBeforeSlider;

		// we need to reserve those two values if the slider starts far
		if (startFar) {
			numLedsBeforeSlider = numLedsAfterSlider;
			numLedsAfterSlider = step;
		}

		// add the rendering instructions for the pre-slider background colour, 
		// slider colour, post-slider background colour
		if (numLedsBeforeSlider > 0) {
			output->SetNextRenderingInstruction(&backgroundColour, numLedsBeforeSlider);
		}
		output->SetNextRenderingInstruction(&sliderColour, sliderWidth);
		if (numLedsAfterSlider > 0) {
			output->SetNextRenderingInstruction(&backgroundColour, numLedsAfterSlider);
		}
	}
}