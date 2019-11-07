#include "LPE.h"

namespace LS{
	/*!
	  @brief   Gets the Light Program Instruction (LPI) object instance that represents the string
			   of the LPI.
	  @param   instructionBuffer	 A pointer to the FixedSizeCharBuffer that contains the LPI.
	  @returns An LPI instance for the LPI or nullptr if the instruction is: (a) not valid or (b) not recognised (i.e. not a known instruction)
	*/
	LPI* LPE::GetLPI(FixedSizeCharBuffer* instructionBuffer, bool validate) {
		if (instructionBuffer == nullptr) return nullptr;

		LPI* lpi = nullptr;

		// Extract the command parameters so we can detemrine
		// which LPI to execute
		char* pLpi = instructionBuffer->GetBuffer();
		bool isValid = this->stringProcessor->ExtractLPIFromHexEncoded(pLpi, currentLPIInstruction);
		// LPIInstruction lpiInstruction = this->stringProcessor->ExtractLPIFromHexEncoded(pLpi, isValid);
		if (isValid == false) return nullptr;
		currentLPIInstruction->lpi = instructionBuffer;

		// Factory which gets the correct LPI instance based on the op-code
		switch (currentLPIInstruction->opcode) {
			case LPI_Clear:
			case LPI_Solid:
			case LPI_Pattern:
			case LPI_Slider:
			case LPI_Fade:
			case LPI_Stochastic:
			case LPI_Blocks:
				lpi = lpis[currentLPIInstruction->opcode];
				break;
		}

		if (lpi == nullptr) return nullptr;

		// Initialise the LPI with the details
		// from the instructionBuffer
		bool reset = lpi->Reset(this->currentLPIInstruction, validate);
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

		// Reset the properties that track the program execution
		this->lpValid = false;
		this->currentIns = nullptr;
		this->loopIterationsRemaining = 0;
		this->loopsRemaining[0] = this->loopsRemaining[1] = this->loopsRemaining[2] = this->loopsRemaining[3] = this->loopsRemaining[4] = this->loopsRemaining[5] = 0;
		this->loopsLPIndices[1] = this->loopsLPIndices[2] = this->loopsLPIndices[3] = this->loopsLPIndices[4] = this->loopsLPIndices[5] = 0;
		this->loopsLPIndices[1] = 1;	// first set of instructions is always going to be placed beginning at 1
		this->instructionsDepth = 0;
		this->infiniteLoopCounter = 0;
		// this->currentInstruction = nullptr;

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
		// this->infiniteLoopCounter = 0;
		// instructionsDepth = 0;
		this->VerifyInstructions(&instructions, result);
		this->lpValid = result->GetCode() == Valid;

		//this->currentIterator = instructions.begin();
		//this->currentInstructions = &instructions;
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
		
		// Ensure we are correctly tracking the depth of the instructions element
		if (currentIns == nullptr) {
			instructionsDepth++;
		}

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
				else if (this->currentIns == nullptr) {
					// Set the first LPI to be executed
					// this->currentInstruction = instructionObject;
					this->currentIns = val;
					this->loadLpi = true;
					this->currentIterator = instructionsObject->begin();
					// this->currentInstructions = instructionsObject;
					// this->currentLPI = lpi;
					// this->currentInstructionRemainingPulses = lpiInstruction.duration * lpi->GetTotalNumberOfSteps();
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

		// Set the number of iterations for the 1st insruction element
		if (this->currentIns == nullptr) {
			this->loopIterationsRemaining = (times == 0 ? -1 : times);
			this->loopsRemaining[this->instructionsDepth - 1] = (times == 0 ? -1 : times);
			this->loopsLPIndices[this->instructionsDepth - 1] = 1;
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
		// else if (this->currentInstruction == nullptr) {
		//else if (this->currentIns == nullptr) {
		//	// Set the first LPI to be executed
		//	// this->currentInstruction = instructionObject;
		//	this->currentIns = ins;
		//	this->loadLpi = true;
		//	// this->currentLPI = lpi;
		//	// this->currentInstructionRemainingPulses = lpiInstruction.duration * lpi->GetTotalNumberOfSteps();
		//}

		return true;
	}

	bool LPE::GetNextRI(FixedSizeCharBuffer* riBuffer) {
		//if (this->lpValid == false 
		//	|| this->currentInstruction == nullptr
		//	|| this->currentInstruction->isNull()) return false;
		if (this->lpValid == false ||
			this->currentIns == nullptr) return false;

		// if (this->currentInstruction
		if (this->currentIns != nullptr
			&& this->loadLpi == true) {
			// && this->currentLPI == nullptr) {
			// Load the LPI ready for execution
			// const char* ins = this->currentInstruction->as<const char*>();
			// this->lpi.LoadFromBuffer(ins);
			this->lpi.LoadFromBuffer(this->currentIns);
			this->currentLPI = this->GetLPI(&this->lpi, false);

			// this->stringProcessor->ExtractLPIFromHexEncoded(ins, this->currentLPIInstruction);
			this->stringProcessor->ExtractLPIFromHexEncoded(this->currentIns, this->currentLPIInstruction);
			this->currentLPIInstruction->lpi = &this->lpi;
			this->currentLPI->ResetNoValidate(this->currentLPIInstruction);
			this->currentInstructionRemainingPulses = this->currentLPIInstruction->duration * this->currentLPI->GetTotalNumberOfSteps();
			this->pulseCounter = -1;

			this->loadLpi = false;
		}

		// Process the current instruction
		bool nextRi = false;
		if (this->currentInstructionRemainingPulses-- > 0) {
			if (++pulseCounter == 0) {
				riBuffer->ClearBuffer();
				nextRi = this->currentLPI->GetNextRI(riBuffer);
			}
			
			if (pulseCounter == this->currentLPIInstruction->duration - 1) {
				pulseCounter = -1;
			}
		}

		if (this->currentInstructionRemainingPulses == 0) {
			const char* curKey = this->currentIterator->key().c_str();
			const char* curVal = this->currentIterator->value().as<const char*>();

			++this->currentIterator;

			ProcessNextIteratorElement();
		}

		return true;
	}

	const char* LPE::PopToNextInstruction() {
		const char* key = nullptr;
		
		int insPosition = 0;

		// see if we are in a loop and wether there are any remaining iterations
		// in the loop
		
		
		//if (this->loopsRemaining[this->instructionsDepth - 2] == -1
		//	|| --this->loopsRemaining[this->instructionsDepth - 2] > 0) {
		//}

		if ( // loopsRemaining[instructionsDepth - 2] != -1
			loopsRemaining[this->instructionsDepth - 2] == 0
			|| --loopsRemaining[this->instructionsDepth - 2] == 0) {
		// else {
			// Pop the loop stack - i.e. reverse one up
			this->instructionsDepth--;

			// 'JUMP' to the correct instruction with this new block (if last then we need to
			// work out how far back we need to go.
			// insPosition = loopsLPIndices[instructionsDepth - 2];
			insPosition = loopsLPIndices[instructionsDepth - 1];

			// Reset current depth to 0 as we have popped back
			this->loopsLPIndices[this->instructionsDepth] = 0;


			// We also need to decrement that loop's remaing loops (if not infinite)
			// as we have now completed a single iteration.
			// THIS DOES NOT WORK BECAUSE WE MAY NOT BE AT THE END OF 'POPPED' LOOP
			//if (--instructionsDepth > 1 
			//	&& this->loopsRemaining[this->instructionsDepth - 2] != -1) {
			//	this->loopsRemaining[this->instructionsDepth - 2]--;
			//}
		}

		// Navigate the document until we get to the correct "instructions" element
		JsonObject currentInstructionsElement = lpeDoc["instructions"];

		for (int i = 0; i < instructionsDepth - 1; i++) {
			// ensure we are positioned at the correct repeat element
			currentIterator = currentInstructionsElement.begin();
			for (int j = 0; j < loopsLPIndices[i] - 1; j++) {
				++currentIterator;
			}

			const char* key2 = this->currentIterator->key().c_str();
			JsonObject containerRepeatElement = currentIterator->value();

			// JsonObject containerRepeatElement = currentInstructionsElement["repeat"];	

			// Get the instructions element of the repeat element
			currentInstructionsElement = containerRepeatElement["instructions"];
		}



		// Reset the LP instruction pointer to 0 for the new current instructions block
		// THIS HAS BEEN COMMENTED OUT!!!
		this->loopsLPIndices[this->instructionsDepth - 1] = 0;

		// Reset the iterator to the current instructions element
		currentIterator = currentInstructionsElement.begin();

		// Process the first element of this instructions element by setting the key
		// to the first element
		key = this->currentIterator->key().c_str();

		// Move the instruction ahead to the next instruction if the loop was 'popped'
		if (insPosition > 0) {
			for (int i = 0; i < insPosition; i++) {
				++currentIterator;
				this->loopsLPIndices[this->instructionsDepth - 1]++;
			}
			key = this->currentIterator->key().c_str();

			if (key == nullptr) {
				if (instructionsDepth == 1) {
					// We've reacehd the end of the program
					return nullptr;
				}

				// we are at the end of the loop, so we need to decrement this current loops remaing iterations
				if (instructionsDepth > 1
					&& this->loopsRemaining[this->instructionsDepth - 2] != -1
					// &&  --this->loopsRemaining[this->instructionsDepth - 2] == 0))
					&& (this->loopsRemaining[this->instructionsDepth - 2] == 0 || --this->loopsRemaining[this->instructionsDepth - 2] == 0)) {
					// there are no further loops remaing, so we need to 'pop' to the next
					// instructions element

					// TODO: pop-up yet further - keep doing so until we have an instruction of
					// are at the top

					key = PopToNextInstruction();
				}
				else {
					// further iterations so simply position at first instruction of the loop
					currentIterator = currentInstructionsElement.begin();
					key = this->currentIterator->key().c_str();

					// ensure we reset the position of LP pointer for this new collection
					// should be the first instruction again!
					this->loopsLPIndices[this->instructionsDepth - 1] = 0;
				}
			}
		}

		return key;
	}

	void LPE::ProcessNextIteratorElement() {
		const char* key = this->currentIterator->key().c_str();

		// this->currentLPI == 0x0;
		// if (key == nullptr) {
		if (key == nullptr && instructionsDepth > 1) {
			key = PopToNextInstruction();

			//// end element.  Pop stack of loops?
			//// this->currentInstruction = nullptr;

			//int insPosition = 0;

			//// see if we are in a loop and wether there are any remaining iterations
			//// in the loop
			//// this->loopsRemaining[this->instructionsDepth - 1] = (times == 0 ? -1 : times);
			//// if (this->loopIterationsRemaining == -1
			//// 	|| --loopIterationsRemaining > 0) {
			//if(this->loopsRemaining[this->instructionsDepth - 2] == -1
			//	|| --this->loopsRemaining[this->instructionsDepth - 2] > 0) {
			//	// Iterations remaining so we don't need to move back up
			//}
			//else {
			//	// Pop the loop stack - i.e. reverse one up
			//	this->instructionsDepth--;

			//	// 'JUMP' to the correct instruction with this new block (if last then we need to
			//	// work out how far back we need to go.
			//	// insPosition = loopsLPIndices[instructionsDepth - 2];
			//	insPosition = loopsLPIndices[instructionsDepth - 1];

			//	// Reset current depth to 0 as we have popped back
			//	this->loopsLPIndices[this->instructionsDepth] = 0;


			//	// We also need to decrement that loop's remaing loops (if not infinite)
			//	// as we have now completed a single iteration.
			//	// THIS DOES NOT WORK BECAUSE WE MAY NOT BE AT THE END OF 'POPPED' LOOP
			//	//if (--instructionsDepth > 1 
			//	//	&& this->loopsRemaining[this->instructionsDepth - 2] != -1) {
			//	//	this->loopsRemaining[this->instructionsDepth - 2]--;
			//	//}
			//}

			//// Navigate the document until we get to the correct "instructions" element
			//JsonObject currentInstructionsElement = lpeDoc["instructions"];

			//for (int i = 0; i < instructionsDepth - 1; i++) {
			//	// ensure we are positioned at the correct repeat element
			//	currentIterator = currentInstructionsElement.begin();
			//	for (int j = 0; j < loopsLPIndices[i] - 1; j++) {
			//		++currentIterator;
			//	}

			//	const char* key2 = this->currentIterator->key().c_str();
			//	JsonObject containerRepeatElement = currentIterator->value();

			//	// JsonObject containerRepeatElement = currentInstructionsElement["repeat"];	

			//	// Get the instructions element of the repeat element
			//	currentInstructionsElement = containerRepeatElement["instructions"];
			//}



			//// Reset the LP instruction pointer to 0 for the new current instructions block
			//// THIS HAS BEEN COMMENTED OUT!!!
			//this->loopsLPIndices[this->instructionsDepth - 1] = 0;

			//// Reset the iterator to the current instructions element
			//currentIterator = currentInstructionsElement.begin();

			//// Process the first element of this instructions element by setting the key
			//// to the first element
			//key = this->currentIterator->key().c_str();

			//// Move the instruction ahead to the next instruction if the loop was 'popped'
			//if (insPosition > 0) {
			//	for (int i = 0; i < insPosition; i++) {
			//		++currentIterator;
			//		this->loopsLPIndices[this->instructionsDepth - 1]++;
			//	}
			//	key = this->currentIterator->key().c_str();

			//	if (key == nullptr) {
			//		// we are at the end of the loop, so we need to decrement this current loops remaing iterations
			//		if(instructionsDepth > 1 
			//			&& this->loopsRemaining[this->instructionsDepth - 2] != -1
			//			&& --this->loopsRemaining[this->instructionsDepth - 2] == 0) {
			//			// there are no further loops remaing, so we need to 'pop' to the next
			//			// instructions element

			//			// TODO: pop-up yet further - keep doing so until we have an instruction of
			//			// are at the top
			//		}
			//		else {
			//			// further iterations so simply position at first instruction of the loop
			//			currentIterator = currentInstructionsElement.begin();
			//			key = this->currentIterator->key().c_str();
			//			
			//			// ensure we reset the position of LP pointer for this new collection
			//			// should be the first instruction again!
			//			this->loopsLPIndices[this->instructionsDepth - 1] = 0;
			//		}
			//	}
			//}


		}

		if(key == nullptr) {
			this->currentIns = nullptr;
		}
		else {
			// See what the next instruction is...
			if (strcmp(key, "instruction") == 0) {
				// this->loopsLPIndices[this->instructionsDepth - 2]++;
				this->loopsLPIndices[this->instructionsDepth - 1]++;

				// 					this->currentInstruction = this->currentIterator->value();
				JsonVariant newInstruction = this->currentIterator->value();
				// this->currentInstruction = &newInstruction;
				this->currentIns = newInstruction.as<const char*>();
				this->loadLpi = true;
			}
			else if (strcmp(key, "repeat") == 0) {
				// this->loopsLPIndices[this->instructionsDepth - 2]++;
				this->loopsLPIndices[this->instructionsDepth - 1]++;

				JsonVariant repeatInstruction = this->currentIterator->value();
				ProcessRepeatElement(&repeatInstruction);
			}

		}
	}

	void LPE::ProcessRepeatElement(JsonVariant* repeatElement) {
		// get the times property
		int times = (*repeatElement)["times"] | -1;
		this->loopIterationsRemaining = (times == 0 ? -1 : times);
		// this->loopsRemaining[this->instructionsDepth - 1] = (times == 0 ? -1 : times);
		if (this->loopsRemaining[this->instructionsDepth - 1] == 0) {
			this->loopsRemaining[this->instructionsDepth - 1] = (times == 0 ? -1 : times);
		}

		// Set the depth point with the instructions block to 1
		// this->loopsLPIndices[this->instructionsDepth - 1] = 0;
		this->loopsLPIndices[this->instructionsDepth] = 0;

		// get the instructions element
		JsonObject instructions = (*repeatElement)["instructions"];

		// Ensure we increment the position we are in the tree now as these new instructions
		// mean that we are moving 'down'.
		instructionsDepth++;

		// set the new iterator to be the instructions iterator
		// this->currentInstructions = &instructions;
		this->currentIterator = instructions.begin();

		// process instructions of new iterator
		ProcessNextIteratorElement();
	}

}