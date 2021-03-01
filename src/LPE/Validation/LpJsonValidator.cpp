#include "LpJsonValidator.h"

namespace LS {
	/*!
		@brief	Adds the dependendencies required to validate
				Light Programs.
		@param	factory		A pointer to the factory that gets validator instances.
		@author	Kevin White
		@date	18 Dec 2020
	*/
	LpJsonValidator::LpJsonValidator(JsonInstructionValidatorFactory* factory) {
		validatorFactory = factory;
	}

	/*!
		@brief	Validates a collection of individual instruction elements
				that are contained in an array.
		@param	instructions	The JSON array which contains the instructions to be validated.
		@param	result			A pointer to the object that contains the result of verifying the LP.
		@returns	True if the instructions are valid, false otherwise.
		@author	Kevin White
		@date	18 Dec 2020
	*/
	void LpJsonValidator::ValidateInstructions(JsonArray* instructions, LPValidateResult* result) {
		for (JsonVariant value : *instructions) {
			bool isRepeat = value.containsKey("repeat");

			if (isRepeat) {
				// Validate the repeat...
				IJsonInstructionValidator* repeatValidator = validatorFactory->GetValidator(InstructionType::Repeat);
				JsonVariant repeatVariant = value["repeat"];

				// ...basics are OK (has a "times" property and "instructions" array)...
				repeatValidator->Validate(&repeatVariant, result);
				if (result->GetCode() != LPValidateCode::Valid) {
					return;
				}

				// ...an infinite loop is not already present (only one allowed in a program)...
				int times = repeatVariant["times"].as<int>();
				if (times == 0) {
					if (hasInfiniteLoop) {
						result->ResetResult(LPValidateCode::OnlyOneInfiniteLoopAllowed);
						return;
					}
					hasInfiniteLoop = true;
				}

				// ...and, finally, that the instructions array are also valid
				JsonArray repeatInstructions = repeatVariant["instructions"];
				ValidateInstructions(&repeatInstructions, result);
			}
			else {
				// Validate the LPI
				IJsonInstructionValidator* lpiValidator = validatorFactory->GetValidator(InstructionType::Lpi);
				lpiValidator->Validate(&value, result);
			}

			if (result->GetCode() != LPValidateCode::Valid) {
				return;
			}
		}
	}
	
	/*!
		@brief	Validates an entire Light Program according to the following rules:
				1. LPI instructions are valid according to the specific rules for individual LPIs.
				2. Repeat instructions are well formed.
				3. The mandatory basic properties are present: name and instructions.
				4. There is only a single at most infinite loop in a program.
		@param	lp		A pointer to the buffer that contains the Light Program to be validated.
		@param	result	A pointer to the object that contains the result of verifying the LP.
		@returns	True if the Light Program is valid, false otherwise.
		@author	Kevin White
		@date	18 Dec 2020
	*/
	void LpJsonValidator::ValidateLp(FixedSizeCharBuffer* lp, LPValidateResult* result) {
		result->ResetResult(LPValidateCode::Valid);
		hasInfiniteLoop = false;

		if (lp == nullptr) {
			result->ResetResult(LPValidateCode::NoIntructions);
			return;
		}

		// Load the JSON document first from the lp.  Is the JSON well-formed
		// and not too big to fit in the buffer?
		const char* pLp = lp->GetBuffer();
		DeserializationError error = deserializeJson(validateJsonDoc, pLp);
		if (error != error.Ok) {
			if (error == error.NoMemory) {
				result->ResetResult(LPValidateCode::ProgramTooBig, nullptr);
			}
			else {
				result->ResetResult(LPValidateCode::MissingMandatoryProperties, nullptr);
			}
			return;
		}

		// Validate basic details: has a name property and collection
		// of instructions in an array
		// { "name" : "program name", "instructions": [ "00010000" ...] }
		const char* progName = validateJsonDoc["name"];
		if (progName == nullptr || strlen(progName) < 5) {
			result->ResetResult(LPValidateCode::MissingMandatoryProperties, nullptr);
			return;
		}

		JsonArray instructionsArr = validateJsonDoc["instructions"];
		if (instructionsArr.isNull() || instructionsArr.size() == 0) {
			result->ResetResult(LPValidateCode::NoIntructions);
			return;
		}

		ValidateInstructions(&instructionsArr, result);
	}
}