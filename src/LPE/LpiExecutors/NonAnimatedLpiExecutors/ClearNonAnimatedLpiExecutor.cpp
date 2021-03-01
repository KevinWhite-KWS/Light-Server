#include "ClearNonAnimatedLpiExecutor.h"

namespace LS {
	/*!
		@brief		Valites that the LPI string is valid according to the
					rules for the clear instruction.  This will always be true
					as there are no rules for the clear instruction.
		@returns	True as the clear instruction is simple.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	bool ClearNonAnimatedLpiExecutor::ValidateLpi(LpiExecutorParams* lpiExecParams) {
		return true;
	}

	/*!
		@brief		Executes the clear instruction and populates the output.
		@param		lpiExecParams		The basic parametes necessary to execute an instruction.
		@param		output				A pointer to the class that is used to set the pixel
										outputs from executing the instruction.
		@author		Kevin White
		@date		2 Jan 2020
	*/
	void ClearNonAnimatedLpiExecutor::ExecuteNonAnimated(LpiExecutorParams* lpiExecParams, LpiExecutorOutput* output) {
		if (output == nullptr) {
			return;
		}

		// reset the state of the output, including a flag that states that output has been set!
		output->Reset();

		// set the 1 and only rendering instruction to be black and repeat (represented by 0)
		Colour black(0, 0, 0);
		output->SetNextRenderingInstruction(&black, 1);
		output->SetRepeatRenderingInstructions();
	}
}