#include "LPE.h"

namespace LS{
	/*!
	  @brief   Gets the Light Program Instruction (LPI) object instance that represents the string
			   of the LPI.
	  @param   instructionBuffer	 A pointer to the FixedSizeCharBuffer that contains the LPI.
	  @returns An LPI instance for the LPI or nullptr if the instruction is: (a) not valid or (b) not recognised (i.e. not a known instruction)
	*/
	LPI* LPE::GetLPI(FixedSizeCharBuffer* instructionBuffer) {
		if (instructionBuffer == nullptr) return nullptr;

		LPI* lpi = nullptr;

		// Extract the command parameters so we can detemrine
		// which LPI to execute
		char* pLpi = instructionBuffer->GetBuffer();
		bool isValid = false;
		LPIInstruction lpiInstruction = this->stringProcessor->ExtractLPIFromHexEncoded(pLpi, isValid);
		if (isValid == false) return nullptr;
		lpiInstruction.lpi = instructionBuffer;

		// Factory which gets the correct LPI instance based on the op-code
		switch (lpiInstruction.opcode) {
			case LPI_Clear:
			case LPI_Solid:
			case LPI_Pattern:
			case LPI_Slider:
			case LPI_Fade:
			case LPI_Stochastic:
			case LPI_Blocks:
				lpi = lpis[lpiInstruction.opcode];
				break;
		}

		if (lpi == nullptr) return nullptr;

		// Initialise the LPI with the details
		// from the instructionBuffer
		bool reset = lpi->Reset(&lpiInstruction);
		if (reset == false) return nullptr;

		return lpi;
	}

	/*!
	  @brief   Validates a Light Program (LP).
	  @param   lp			 A pointer to the FixedSizeCharBuffer that contains the LP.
	  @param   result		 A pointer to LPValidateResult instance that will contain the result of performing the validation.
	*/
	void LPE::ValidateLP(FixedSizeCharBuffer* lp, LPValidateResult* result) {
		if (lp == nullptr || result == nullptr) return;

		result->ResetResult(Valid);

		// Deserialise the lp buffer to JSON using Arduino.JSON library
		DeserializationError error = deserializeJson(this->lpeDoc, lp->GetBuffer());

		// Most errors should return MissingMAndatoryProperties
		if (error != error.Ok) {
			if (error == error.NoMemory) {
				result->ResetResult(ProgramTooBig, nullptr);
			}
			else {
				result->ResetResult(MissingMandatoryProperties, nullptr);
			}
			return;
		}

		// Get the mandatory name property
		const char* progName = this->lpeDoc["name"];
		if (progName == nullptr || strlen(progName) < 5) {
			result->ResetResult(MissingMandatoryProperties, nullptr);
			return;
		}

		// Get the mandatory instructions property
		JsonObject instructions = this->lpeDoc["instructions"];
		if (instructions.isNull() == true) {
			result->ResetResult(NoIntructions, nullptr);
			return;
		}

		// Validate the LP by beginning with validation of the instructions
		// object
		this->infiniteLoopCounter = 0;
		this->VerifyInstructions(&instructions, result);
	}

	/*!
	  @brief   Verifies that an "instructions" property conforms to the following rules: (a)
				has at least one "instruction" or "repeat" members.
	  @param   instructionsObject	 A pointer to the the JsonObject that contains the "instructions" property.
	  @param   result				 A pointer to the LPValidateResult object instance which stores the result of the validation.
	  @returns True if the "instructions" property conforms to the rules or false otherwise.
	*/
	bool LPE::VerifyInstructions(JsonObject* instructionsObject, LPValidateResult* result) {
		uint8_t countValidInstructions = 0;
		for (JsonObject::iterator it = instructionsObject->begin(); it != instructionsObject->end(); ++it) {
			const char* key = it->key().c_str();
			const char* val = it->value().as<char*>();

			if (strcmp(key, "instruction") == 0) {
				countValidInstructions++;
				// verify the instruction
				JsonVariant instructionObject = it->value();
				if (this->VerifyInstruction(&instructionObject, result) == false) {
					return false;
				}
			}
			else if (strcmp(key, "repeat") == 0) {
				countValidInstructions++;
				// Ensure there are not more than 5 nested loops
				if (++nestedLoopCounter > 5) {
					result->ResetResult(Maximum5NestedLoopsAllowed, nullptr);
					return false;
				}
				// verify the repeat
				JsonObject repeatObject = it->value();
				if (this->VerifyRepeat(&repeatObject, result) == false) {
					return false;
				}
				this->nestedLoopCounter--;
			}
			else {
				result->ResetResult(InvalidProperty, key);
				return false;
			}
		}

		if (countValidInstructions == 0) {
			// Must be at least one instruction or repeat
			result->ResetResult(NoIntructions, nullptr);
			return false;
		}

		return true;
	}

	/*!
	  @brief   Verifies that an "repeat" property conforms to the following rules:
			   (a) has a "times" member, (b) the times member has a value between 0 and 1000,
			   (c) that there is not another infiinite loop if this is an infinite loop,
			   (d) has the mandatory "instructions" property.
	  @param   instructionsObject	 A pointer to the the JsonObject that contains the "repeat" property.
	  @param   result				 A pointer to the LPValidateResult object instance which stores the result of the validation.
	  @returns True if the "repeat" property conforms to the rules or false otherwise.
	*/
	bool LPE::VerifyRepeat(JsonObject* repeatObject, LPValidateResult* result) {


		// Get the time property and ensure it is a value between 0 and 1000
		JsonVariant  timesProperty = repeatObject->getMember("times");
		if (timesProperty.isNull() == true) {
			result->ResetResult(MissingMandatoryProperties, nullptr);
			return false;
		}

		int times = (*repeatObject)["times"] | -1;
		if (times < 0 || times > 1000) {
			const char* timesStr = (*repeatObject)["times"];
			if (timesStr == nullptr) {
				result->SetInfoFromInt(times);
				result->ResetResult(LoopHasInvalidTimesValue);
			}
			else {
				result->ResetResult(LoopHasInvalidTimesValue, timesStr);
			}
			return false;
		}

		// Ensure there is only a single infinite loop
		if (times == 0 && ++this->infiniteLoopCounter > 1) {
			result->ResetResult(OnlyOneInfiniteLoopAllowed, nullptr);
			return false;
		}

		// Get the mandatory instructions property
		JsonObject instructions = (*repeatObject)["instructions"];
		if (instructions.isNull() == true) {
			result->ResetResult(NoInstructionsInLoop, nullptr);
			return false;
		}

		// Validate the instructions object
		return this->VerifyInstructions(&instructions, result);
	}

	/*!
	  @brief   Verifies that an individual LPI is valid according to the specific
			   rules of that LPI.  This is acheived by initialising the instance
			   of the LPI and then calling the Reset member.  If the LPI is not recognised
			   then false is returned.
	  @param   instructionsObject	 A pointer to the the JsonObject that contains the "instruction" property.
	  @param   result				 A pointer to the LPValidateResult object instance which stores the result of the validation.
	  @returns True if the "instruction" property conforms to the rules or false otherwise.
	*/
	bool LPE::VerifyInstruction(JsonVariant* instructionObject, LPValidateResult* result) {
		const char *ins = instructionObject->as<const char*>();

		if (ins == nullptr) {
			result->ResetResult(InvalidInstruction, nullptr);
			return false;
		}

		// Get the respective instruction instance
		this->lpi.LoadFromBuffer(ins);
		LPI *lpi = this->GetLPI(&this->lpi);

		if (lpi == nullptr) {
			// No a valid instruction!
			result->ResetResult(InvalidInstruction, ins);
			return false;
		}

		// Validate the instruction
		bool isValid;
		LPIInstruction lpiInstruction = this->stringProcessor->ExtractLPIFromHexEncoded(ins, isValid);
		lpiInstruction.lpi = &this->lpi;
		isValid = lpi->Reset(&lpiInstruction);
		
		if (lpi->Validate() == false) {
			result->ResetResult(InvalidInstruction, ins);
			return false;
		}

		return true;
	}

	bool LPE::GetNextRI(FixedSizeCharBuffer* riBuffer) {
		return false;
	}


}