#include "NonAnimatedLpiExecutor.h"

namespace LS {
	/*!
		@brief		Gets the number of steps which will always be 1 for non-animated LPIs.
		@returns	The number of steps to render the LPI - always 1 for non-animated LPIs.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	uint16_t NonAnimatedLpiExecutor::GetNumberOfSteps(LpiExecutorParams* lpiExecParams) {
		return 1;
	}

	/*!
		@brief		Executes the instruction and populates the output.
		@param		lpiExecParams		The basic parametes necessary to execute an instruction.
		@param		step				The step number to execute the LPI.  This is ignore for
										non-animated LPIs.
		@param		output				A pointer to the class that is used to set the pixel
										outputs from executing the instruction.
		@author		Kevin White
		@date		2 Jan 2020
	*/
	void NonAnimatedLpiExecutor::Execute(LpiExecutorParams* lpiExecParams, uint16_t step, LpiExecutorOutput* output) {
		if (output == nullptr) {
			return;
		}

		ExecuteNonAnimated(lpiExecParams, output);
	}
}