#include "FadeAnimatedLpiExecutor.h"

namespace LS {
	/*!
		@brief		Valites that the LPI string is valid according to the
					rules for the fade instruction.
		@param		lpiExecParams		The basic parametes necessary to execute an instruction.
		@returns	True as the clear instruction is simple.
		@author		Kevin White
		@date		4 Jan 2021
	*/
	bool FadeAnimatedLpiExecutor::ValidateLpi(LpiExecutorParams* lpiExecParams) {
		if (lpiExecParams == nullptr) {
			return false;
		}

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();
		StringProcessor* stringProcessor = lpiExecParams->GetStringProcesor();

		// we should have a number specifying the step of the fade effect
		bool lpiIsValid = false;
		uint8_t stepValue = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 50, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// next, we should have a boolean for whether we need to fade in (0) or fade out (1)
		lpiBuffer += 2;
		bool fadeIn = stringProcessor->ExtractBoolFromHexEncoded(lpiBuffer, lpiIsValid);
		if (!lpiIsValid) {
			return false;
		}

		// finally, we should have two colours - first one for start colour and one for the end colour
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
			@brief		Gets the number of animation steps to complete the fade
						effect for a specified slider LPI.
			@param		lpiExecParams		The basic parametes necessary to execute an instruction.
			@returns	The number of animation steps of the slider effect.
			@
	*/
	uint16_t FadeAnimatedLpiExecutor::GetNumberOfSteps(LpiExecutorParams* lpiExecParams) {
		if (lpiExecParams == nullptr) {
			return false;
		}

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();
		StringProcessor* stringProcessor = lpiExecParams->GetStringProcesor();

		// we need the step size and two colours in order to calculate the total steps
		bool isValid;
		uint8_t stepValue = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 50, isValid);
		bool fadeOut = stringProcessor->ExtractBoolFromHexEncoded(lpiBuffer + 2, isValid);
		Colour startColour = stringProcessor->ExtractColourFromHexEncoded(lpiBuffer + 3, isValid);
		Colour endColour = stringProcessor->ExtractColourFromHexEncoded(lpiBuffer + 9, isValid);

		// if fade out then simply switch the end / start colours
		if (fadeOut) {
			Colour tempColour = startColour;
			startColour = endColour;
			endColour = tempColour;
		}

		int redDiff = endColour.red - startColour.red;
		int greenDiff = endColour.green - startColour.green;
		int blueDiff = endColour.blue - startColour.blue;

		// the number of steps is the biggest pixel different between any of the colour
		// components / by the step value
		int maxDiff = max(redDiff, greenDiff);
		maxDiff = max(maxDiff, blueDiff);
		double steps = (double)maxDiff / (double)stepValue;
		if (steps < 0) steps = 0;
		uint16_t totalSteps = ceil(steps) + 1;

		return totalSteps;
	}

	/*!
		@brief		Executes the fade instruction and populates the output.
		@param		lpiExecParams		The basic parametes necessary to execute an instruction.
		@param		step				The step number in the animation which is to be rendered.  For example:
										if the slider has 5 animation steps and step is 2 then the 2nd
										animation step will be rendered.
		@param		output				A pointer to the class that is used to set the pixel
										outputs from executing the instruction.
		@author		Kevin White
		@date		4 Jan 2020
	*/
	void FadeAnimatedLpiExecutor::Execute(LpiExecutorParams* lpiExecParams, uint16_t step, LpiExecutorOutput* output) {
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
		uint8_t stepValue = stringProcessor->ExtractNumberFromHexEncoded(lpiBuffer, 1, 50, isValid);
		bool fadeOut = stringProcessor->ExtractBoolFromHexEncoded(lpiBuffer + 2, isValid);
		Colour startColour = stringProcessor->ExtractColourFromHexEncoded(lpiBuffer + 3, isValid);
		Colour endColour = stringProcessor->ExtractColourFromHexEncoded(lpiBuffer + 9, isValid);

		// calculate the new colour components
		
		/*Colour firstColour = (!fadeOut ? startColour : endColour);
		Colour secondColour = (!fadeOut ? endColour : startColour);*/
		/*Colour firstColour = startColour;
		Colour secondColour = endColour;*/
		long stepVal = (!fadeOut ? step : -1 * step);

		int newR = startColour.red + stepVal * stepValue;
		int newG = startColour.green + stepVal * stepValue;
		int newB = startColour.blue + stepVal * stepValue;
		Colour fadeColour(0,0,0);
		if (!fadeOut) {
			fadeColour.red = min(newR, endColour.red);	// WAS secondColour
			fadeColour.green = min(newG, endColour.green);
			fadeColour.blue = min(newB, endColour.blue);
		} else{
			fadeColour.red = max(newR, endColour.red);
			fadeColour.green = max(newG, endColour.green);
			fadeColour.blue = max(newB, endColour.blue);
		}

		// output->SetNextRenderingInstruction(&fadeColour, lpiExecParams->GetLedConfig()->numberOfLEDs);
		output->SetNextRenderingInstruction(&fadeColour, 1);
		output->SetRepeatRenderingInstructions();
	}
}