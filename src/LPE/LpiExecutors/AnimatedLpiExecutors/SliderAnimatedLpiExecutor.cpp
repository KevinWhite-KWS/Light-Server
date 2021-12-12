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
		stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 50, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// next, we should have a boolean for whether we need to start near (0) or far (1)
		lpiBuffer += 2;
		bool startNear = stringProcessor->ExtractBoolFromHexEncoded(lpiBuffer, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// now we check that a value has been specified for the head length (0 - 100)
		lpiBuffer += 1;
		stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 0, 100, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// also, check for a valid tail length (0 - 100)
		lpiBuffer += 2;
		stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 0, 100, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// finally, we should have two colours - first one for slider and second for background
		lpiBuffer += 2;
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
		uint8_t headLength = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer + 3, 0, 100, isValid);
		uint8_t tailLength = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer + 5, 0, 100, isValid);
		Colour sliderColour = stringProcessor->ExtractColourFromHexEncoded(lpiBuffer + 7, isValid);
		Colour backgroundColour = stringProcessor->ExtractColourFromHexEncoded(lpiBuffer + 13, isValid);

		// calculate the values that determine the number of LEDs to be rendered
		uint16_t numLedsBeforeSlider = step;
		uint16_t numLedsAfterSlider = lpiExecParams->GetLedConfig()->numberOfLEDs - sliderWidth - numLedsBeforeSlider;

		// we need to reserve those two values if the slider starts far
		if (startFar) {
			numLedsBeforeSlider = numLedsAfterSlider;
			numLedsAfterSlider = step;
		}

		// render the part before the slider
		if (numLedsBeforeSlider > 0) {
			// render an optional graduated tail
			if (tailLength > 0) {
				RenderTail(numLedsBeforeSlider, lpiExecParams, output, sliderWidth, tailLength, backgroundColour, sliderColour);
			}
			else {
				output->SetNextRenderingInstruction(&backgroundColour, numLedsBeforeSlider);
			}
		}

		// render the slider
		output->SetNextRenderingInstruction(&sliderColour, sliderWidth);

		// render the part after the slider
		if (numLedsAfterSlider > 0) {
			// render an optional graduated head
			if (headLength > 0) {
				RenderHead(numLedsAfterSlider, lpiExecParams, output, sliderWidth, headLength, backgroundColour, sliderColour);
			}

			if (numLedsAfterSlider > 0) {
				output->SetNextRenderingInstruction(&backgroundColour, numLedsAfterSlider);
			}
		}
	}

	/*!
		@brief		Render a graduated tail (the graduation that comes before the slider).
		@param		numLedsBeforeSlider		The number of pixels that come before the slider
		@param		lpiExecParams			The basic parametes necessary to execute an instruction.
		@param		output					A pointer to the class that is used to set the pixel
											outputs from executing the instruction.
		@param		sliderWidth				The width of the slider in pixels.
		@param		tailLength				The length of the tail in pixels.
		@param		backgroundColour		The background colour of the slider.
		@param		sliderColour			The slider colour.
		@author		Kevin White
		@date		10 March 2021
	*/
	void SliderAnimatedLpiExecutor::RenderTail(
		uint16_t &numLedsBeforeSlider, 
		LpiExecutorParams* lpiExecParams, 
		LpiExecutorOutput* output,
		uint8_t &sliderWidth,
		uint8_t &tailLength,
		Colour &backgroundColour,
		Colour &sliderColour
	) {
		uint16_t numTailPixels = (lpiExecParams->GetLedConfig()->numberOfLEDs - sliderWidth) * ((float)tailLength / 100);
		uint16_t tailStep = min(numLedsBeforeSlider, numTailPixels);
		uint16_t tailPixelsToRender = numLedsBeforeSlider - numTailPixels < 0 ? numLedsBeforeSlider : numTailPixels;

		numLedsBeforeSlider = numLedsBeforeSlider - tailPixelsToRender;
		if (numLedsBeforeSlider > 0) {
			output->SetNextRenderingInstruction(&backgroundColour, numLedsBeforeSlider);
		}

		gradientEffect.Reset(sliderColour, backgroundColour, numTailPixels);
		Colour newTailColour;
		for (int i = 0; i < tailPixelsToRender; i++) {
			gradientEffect.CalculatetepColour(tailStep--, newTailColour);
			output->SetNextRenderingInstruction(&newTailColour, 1);
		}
	}

	/*!
		@brief		Render a graduated head (the graduation that comes after the slider).
		@param		numLedsAfterSlider		The number of pixels that come after the slider
		@param		lpiExecParams			The basic parametes necessary to execute an instruction.
		@param		output					A pointer to the class that is used to set the pixel
											outputs from executing the instruction.
		@param		sliderWidth				The width of the slider in pixels.
		@param		headLength				The length of the head in pixels.
		@param		backgroundColour		The background colour of the slider.
		@param		sliderColour			The slider colour.
		@author		Kevin White
		@date		10 March 2021
	*/
	void SliderAnimatedLpiExecutor::RenderHead(
		uint16_t& numLedsAfterSlider,
		LpiExecutorParams* lpiExecParams,
		LpiExecutorOutput* output,
		uint8_t& sliderWidth,
		uint8_t& headLength,
		Colour& backgroundColour,
		Colour& sliderColour
	) {
		uint16_t numHeadPixels = (lpiExecParams->GetLedConfig()->numberOfLEDs - sliderWidth) * ((float)headLength / 100);

		gradientEffect.Reset(sliderColour, backgroundColour, numHeadPixels);
		Colour newHeadColour;
		if (numHeadPixels > numLedsAfterSlider) numHeadPixels = numLedsAfterSlider;
		for (int i = 1; i < numHeadPixels + 1; i++) {
			gradientEffect.CalculatetepColour(i, newHeadColour);
			output->SetNextRenderingInstruction(&newHeadColour, 1);
			numLedsAfterSlider--;
		}
	}
}