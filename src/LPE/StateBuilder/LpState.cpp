#include "LpState.h"

namespace LS {
	/*!
		@brief	Resets the state back to the initial state, in preparation
				for a new LP to be executed.
		@author	Kevin White
		@date	21 Dec 2020
	*/
	void LpState::reset() {
		// reset the state of each of the LP instructions
		for (uint8_t instructionIndex = 0; instructionIndex < MAX_LPINSTRUCTIONS; instructionIndex++) {
			lpInstructions[instructionIndex].reset();
		}

		// reset the state of each of the repeat instructions
		for (uint8_t repeatIndex = 0; repeatIndex < MAX_REPEATINSTRUCTIONS; repeatIndex++) {
			repeatInstructions[repeatIndex].reset();
		}

		// reset the instruction pointers back to defaults, nullptr
		firstInstruction = nullptr;
		currentInstruction = nullptr;

		// reset the index values back to 0
		lpInstructionIndex = 0;
		repeatIndex = 0;
	}

	/*!
		@brief		Returns a pointer to the first instruction
					in the program.
		@author		Kevin White
		@date		21 Dec 2020
	*/
	Instruction* LpState::getFirstInstruction() {
		return firstInstruction;
	}

	/*!
		@brief		Returns a pointer to the current instruction
					that is being executed from the program.
		@author		Kevin White
		@date		21 Dec 2020
	*/
	Instruction* LpState::getCurrentInstruction() {
		return currentInstruction;
	}

	/*!
		@brief		Sets a pointer to the current instruction that
					is being executed in the program.
		@author		Kevin White
		@date		21 Dec 2020
	*/
	void LpState::setCurrentInstruction(Instruction* currentInstruction) {
		// do we need to check that currentInstruction is one
		// of the instructions in the current program?
		this->currentInstruction = currentInstruction;
	}

	/*!
		@brief		Adds a new repeat instruction to the program state.
		@param		repeatInstruction	A pointer to the repeatInstruction that will
										be used to initialise a repeatInstruction in the
										collection of repeat instructions.
		@returns	A pointer to the RepeatInstruction or nullptr if adding
					a new repeat instructon would exceed the maximum number
					of allowable repeat instructions.
		@author		Kevin White
		@date		21 Dec 2020
	*/
	Instruction* LpState::addRepeatInstruction(RepeatInstruction* repeatInstruction) {
		if (repeatIndex >= MAX_REPEATINSTRUCTIONS) {
			return nullptr;
		}

		repeatInstructions[repeatIndex++].init(repeatInstruction);

		return &repeatInstructions[repeatIndex - 1];
	}

	/*!
		@brief		Adds a new LP instruction to the program state.
		@param		lpInstruction	A pointer to the lpInstruction that will
									be used to initialise a lpInstruction in the
									collection of lp instructions.
		@returns	A pointer to the LpInstruction or nullptr if adding
					a new lp instructon would exceed the maximum number
					of allowable lp instructions.
		@author		Kevin White
		@date		21 Dec 2020
	*/
	Instruction* LpState::addLpInstruction(LpInstruction* lpInstruction) {
		if (lpInstructionIndex >= MAX_LPINSTRUCTIONS) {
			return nullptr;
		}

		lpInstructions[lpInstructionIndex++].init(lpInstruction);

		return &lpInstructions[lpInstructionIndex - 1];
	}

	/*!
		@brief		Adds a new instruction to the program state.
		@param		newInstruction	A pointer to the instruction to be added
									which will be used to initialise an instruction
									of that type in the program state.
		@returns	A pointer to the new instruction instance or nullptr
					if it cannot be added.
		@author		Kevin White
		@date		21 Dec 2020
	*/
	Instruction* LpState::addInstruction(Instruction* newInstruction) {
		if (newInstruction == nullptr) {
			return nullptr;
		}

		Instruction* addedInstruction = nullptr;
		if (newInstruction->getInstructionType() == InstructionType::Repeat) {
			addedInstruction = addRepeatInstruction((RepeatInstruction*)newInstruction);
		}
		else if (newInstruction->getInstructionType() == InstructionType::Lpi) {
			addedInstruction = addLpInstruction((LpInstruction*)newInstruction);
		}

		if (firstInstruction == nullptr && addedInstruction != nullptr) {
			currentInstruction = firstInstruction = addedInstruction;
		}

		return addedInstruction;
	}
}