#include "RepeatJsonInstructionBuilder.h"

namespace LS {
	/*!
		@brief	Adds a repeat instruction to the program that
				is taken from a JSON input document.
				NOTE: we do not verify the repeat statement here, it
				must already have been verified.
		@param	jsonVar		The JSON variant that represents the repeat instruction.
		@param	state		The Light Program state.
		@author	Kevin White
		@date	23 Dec 2020
	*/
	Instruction* RepeatJsonInstructionBuilder::BuildInstruction(JsonVariant* jsonVar, LpState* state) {
		if (jsonVar == nullptr || state == nullptr) {
			return nullptr;
		}

		// get the number of times the repeat needs to execute
		int times = (*jsonVar)["times"].as<int>();

		// add the repeat to Light Program tree
		repeatInstruction.reset();
		repeatInstruction.setNumberOfIterations(times);
		repeatInstruction.setRemainingIterations(times);
		Instruction* repeatIns = state->addInstruction(&repeatInstruction);

		return repeatIns;
	}
}