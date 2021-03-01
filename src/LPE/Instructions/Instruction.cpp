#include "Instruction.h"

namespace LS {
	/*! 
		@brief		Resets the state of the instruction by setting the pointers to null.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	void Instruction::reset() {
		setNext(nullptr);
		setParent(nullptr);
	}

	/*!
		@brief		Sets the pointer to the next instruction.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	void Instruction::setNext(Instruction* next) {
		this->next = next;
	}

	/*!
		@brief		Gets a pointer to the next instruction in the tree, if any.
		@returns	A pointer to the next Instruction or nullptr if there is no next instruction.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	Instruction* Instruction::getNext() {
		return next;
	}

	/*!
		@brief		Checks whether this instruction has a sibling.
		@returns	True if this instruction has a sibling, false otherwise.
		@author		Kevin White
		@date		29 Dec 2020
	*/
	bool Instruction::HasNext() {
		return next != nullptr;
	}

	/*!
		@brief		Sets the pointer to the parent instruction of this instruction.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	void Instruction::setParent(Instruction* parent) {
		this->parent = parent;
	}

	/*!
		@brief		Gets a pointer to the parent instruction, if any.
		@returns	A pointer to the parent instruction or nullptr if there is no parent.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	Instruction* Instruction::getParent() {
		return parent;
	}

	/*!
		@brief		Checks whether this instruction has a parent
					instruction.
		@returns	True if this instruction has a parent instruction, false
					otherwise.
		@author		Kevin White
		@date		29 Dec 2020
	*/
	bool Instruction::HasParent() {
		return parent != nullptr;
	}
}