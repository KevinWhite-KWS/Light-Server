#include "InstructionWithChild.h"

namespace LS {
	/*!
		@brief		Gets a pointer to the first child element of this instruction.
		@returns	A pointer to the first child element or nullptr if there is no child.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	Instruction* InstructionWithChild::getFirstChild() {
		return firstChild;
	}

	/*!
		@brief		Sets the pointer to the first child element of this instruction.
		@param		firstChild		A pointer to the first child element.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	void InstructionWithChild::setFirstChild(Instruction* firstChild) {
		this->firstChild = firstChild;
	}

	/*!
		@brief		Resets the state of this instruction by setting the first
					child element pointer to nullptr.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	void InstructionWithChild::reset() {
		Instruction::reset();
		firstChild = nullptr;
	}
}