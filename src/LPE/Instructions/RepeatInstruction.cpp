#include "RepeatInstruction.h"

namespace LS {
	/*!
		@brief		Gets the number of iterations specified by the repeat instruction.
		@returns	The number of iterations.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	int RepeatInstruction::getNumberOfIterations() {
		return numberOfIterations;
	}

	/*!
		@brief		Sets the number of iterations specified by the repeat instruction.
		@param		numberOfIterations	The number of iterations specified by the repeat instruction.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	void RepeatInstruction::setNumberOfIterations(int numberOfIterations) {
		this->numberOfIterations = numberOfIterations;
	}

	/*!
		@brief		Gets the number of iterations remaining around the loop.
		@returns	The number of remaining iterations.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	int RepeatInstruction::getRemainingIterations() {
		return remainingIterations;
	}

	/*!
		@brief		Resets the remaining number of iterations to the number
					specified in the repeat instruction.  This normally occurs
					when a repeat instruction is first encountered when navigating
					around a program as it is executed.
		@author		Kevin White
		@date		30 Dec 2020
	*/
	void RepeatInstruction::ResetRemainingIterations() {
		setRemainingIterations(getNumberOfIterations());
	}

	/*!
		@brief		Sets the number of remaining iterations.
		@param		remainingIterations	The number of remaining iterations.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	void RepeatInstruction::setRemainingIterations(int remainingIterations) {
		this->remainingIterations = remainingIterations;
	}

	/*!
		@brief		Decrements the number of remaining iterations by 1
					as long as the remaining iterations isen't already 0.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	int RepeatInstruction::decrementRemainingIterations() {
		if (remainingIterations > 0) {
			this->remainingIterations--;
		}

		return remainingIterations;
	}

	/*!
		@brief		Gets whether this repeat instruction is an infinite
					loop.
		@returns	True if this repeat instruction is infinite or false if it is finite.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	bool RepeatInstruction::isInfinite() {
		return numberOfIterations == 0;
	}

	/*!
		@brief		Resets the state of the repeat back to default values.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	void RepeatInstruction::reset() {
		InstructionWithChild::reset();
		numberOfIterations = -1;
		remainingIterations = -1;
	}

	/*!
		@brief		Initialises the state of this repeat from an existing
					repeat instance.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	void RepeatInstruction::init(RepeatInstruction* repeatInstruction) {
		if (repeatInstruction == nullptr) {
			return;
		}

		numberOfIterations = repeatInstruction->getNumberOfIterations();
		remainingIterations = repeatInstruction->getRemainingIterations();
	}
}