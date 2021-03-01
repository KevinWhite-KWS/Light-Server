#include "LpJsonStateBuilder.h"

namespace LS {
	/*!
		@brief		Constructor sets references to dependencies.
		@param		instructionFactory		A pointer to the factory that gives us instances
											to build individual instruction types.
		@author		Kevin White
		@date		23 Dec 2020
	*/
	LpJsonStateBuilder::LpJsonStateBuilder(JsonInstructionBuilderFactory* instructionFactory) {
		this->instructionFactory = instructionFactory;
	}

	/*!
		@brief		Builds the instruction tree from the values that are
					contained in a JSON array.  This is a recursive method
					that is called each time an instructions array is encountered
					in the Light Program.
		@param		instructions		A point to the JSON array which contains the Light Program instructions.
		@param		state				A pointer to the class that stores the state of the Light Program.
		@param		parentInstruction	A pointer to the instruction that is the parent of the instructions
										contained in the JSON array.
		@author		Kevin White
		@date		23 Dec 2020
	*/
	void LpJsonStateBuilder::BuildInstructions(JsonArray* instructions, LpJsonState* state, InstructionWithChild* parentInstruction) {
		Instruction* prevInstruction = nullptr;
		Instruction* currentInstruction = nullptr;
		IJsonInstructionBuilder* builder = nullptr;

		int len = instructions->size();

		for (JsonVariant value : *instructions) {
			bool isRepeat = value.containsKey("repeat");

			if (isRepeat) {
				builder = instructionFactory->GetInstructionBuilder(InstructionType::Repeat);
				JsonVariant repeatVar = value["repeat"];
				currentInstruction = builder->BuildInstruction(&repeatVar, state);

				// Now, this repeat becomes the parent instruction of the instructions
				// contained in the instructions array of this repeat
				JsonArray repeatInstructions = repeatVar["instructions"];
				BuildInstructions(&repeatInstructions, state, (InstructionWithChild*)currentInstruction);
			}
			else {
				// lpi
				IJsonInstructionBuilder* builder = instructionFactory->GetInstructionBuilder(InstructionType::Lpi);
				currentInstruction = builder->BuildInstruction(&value, state);
			}

			// ensure that we set the parent instruction of the current instruction
			// (if one) so that we can navigate 'back-up' the tree e.g. from the last
			// instruction of a repeat back up to the repeat itself
			currentInstruction->setParent(parentInstruction);

			// also, ensure that the first child of the parent is also set
			// so that we can navigate from, e.g., a repeat to the first
			// instruction of the repreat
			if (parentInstruction != nullptr && parentInstruction->getFirstChild() == nullptr) {
				parentInstruction->setFirstChild(currentInstruction);
			}

			// finally, set the previous instruction's subling instruction
			// to the current instruction.  This allows us to navigate
			// from one instruction to the next one.
			if (prevInstruction) {
				prevInstruction->setNext(currentInstruction);
			}
			prevInstruction = currentInstruction;
		}
	}

	/*!
		@brief		Builds the initial state of a Light Program by constructing
					a tree of Instruction instances to represent the
					instructions to be executed.  These Instruction instaneces
					are linked by parent-child pointers, and child-parent
					pointers.  This allows program execution by navigating
					to the next instruction to be executed.
					NOTE: this class does not perform validation, that should
					have been performed before calling this class.  Any validation
					values will cause this method to fail.
		@param		lp		A pointer to the buffer that contains the Light Program
							in JSON format.
		@param		state	A pointer to the class that stores the tree representation
							of the parsed Light Program.
		@author		Kevin White
		@date		23 Dec 2020
	*/
	bool LpJsonStateBuilder::BuildState(FixedSizeCharBuffer* lp, LpJsonState* state) {
		if (lp == nullptr || state == nullptr) {
			return false;
		}

		// reset the existing state, if any, back to default values
		state->reset();

		// load the JSON document with the JSON string contained in lp
		// ready for processing and building the instruction tree
		const char* pLp = lp->GetBuffer();
		DeserializationError error = deserializeJson(*state->getLpJsonDoc(), pLp);

		// get the initial instructions array which contain
		// at least one LPI or repeat instruction
		JsonArray instructions = (*state->getLpJsonDoc())["instructions"];

		BuildInstructions(&instructions, state, nullptr);

		return true;
	}
}