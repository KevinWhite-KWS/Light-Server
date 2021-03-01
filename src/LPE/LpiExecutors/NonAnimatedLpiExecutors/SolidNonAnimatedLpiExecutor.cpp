#include "SolidNonAnimatedLpiExecutor.h"

namespace LS {
	/*!
			@brief		Valites that the LPI string is valid according to the
						rules for the solid instruction.
			@returns	True if the solid instruction is valid.
			@author		Kevin White
			@date		2 Jan 2021
		*/
	bool SolidNonAnimatedLpiExecutor::ValidateLpi(LpiExecutorParams* lpiExecParams) {
		if (lpiExecParams == nullptr) {
			return false;
		}

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();

		// a solid LPI is valid if a colour has been specified
		bool lpiIsValid = false;
		lpiExecParams->GetStringProcesor()->ExtractColourFromHexEncoded(lpiBuffer, lpiIsValid);

		return lpiIsValid;
	}

	/*!
		@brief		Executes the solid instruction and populates the output.
		@param		lpiExecParams		The basic parametes necessary to execute an instruction.
		@param		output				A pointer to the class that is used to set the pixel
										outputs from executing the instruction.
		@author		Kevin White
		@date		2 Jan 2020
	*/
	void SolidNonAnimatedLpiExecutor::ExecuteNonAnimated(LpiExecutorParams* lpiExecParams, LpiExecutorOutput* output) {
		if (lpiExecParams == nullptr 
			|| output == nullptr) {
			return;
		}

		const char* lpiBuffer = lpiExecParams->GetLpiBufferWithoutBasicDetails();

		// reset the state of the output, including a flag that states that output has been set!
		output->Reset();

		// get the solid colour from the LPI and set that as a repeating instruction
		// in the output collection of rendering instructions
		bool isValid = true;
		Colour solidColour = lpiExecParams->GetStringProcesor()->ExtractColourFromHexEncoded(lpiBuffer, isValid);
		output->SetNextRenderingInstruction(&solidColour, 1);
		output->SetRepeatRenderingInstructions();
	}
}