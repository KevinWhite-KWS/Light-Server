#include "RepeatJsonInstructionValidator.h"

namespace LS {
	/*!
		@brief		Validates the Repeat instruction
		@param		jsonVar		Pointer to the elemnt that contains the repeat
		@returns	True if the repeat statement is valid, false otherwise.
		@author		Kevin White
		@date		14 Dec 2020
	*/
	void RepeatJsonInstructionValidator::Validate(JsonVariant* jsonVar, LPValidateResult* result) {
		if (jsonVar == nullptr) {
			result->ResetResult(LPValidateCode::InvalidInstruction);
			return;
		}

		// ensure the repeat has a 'times' property
		JsonVariant timesVar = jsonVar->getMember("times");
		if (timesVar.isNull() || !timesVar.is<int>()) {
			result->ResetResult(LPValidateCode::LoopHasInvalidTimesValue);
			return;
		}
		int times = timesVar.as<int>();
		if (times < 0 || times > 1000) {
			result->ResetResult(LPValidateCode::LoopHasInvalidTimesValue);
			return;
		}

		// ensure the repeat has a 'instructions' array with at least one value
		JsonArray instructionsArr = (*jsonVar)["instructions"];
		if (instructionsArr.isNull() || instructionsArr.size() == 0) {
			result->ResetResult(LPValidateCode::NoInstructionsInLoop);
			return;
		}
	}
}