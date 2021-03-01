#include "LpInstruction.h"

namespace LS {
	/*!
		@brief		Gets a pointer to the LPI.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	const char* LpInstruction::getLpi() {
		return lpi;
	}

	/*!
		@brief		Sets a pointer to the LPI.
		@param		lpi		A pointer to the LPI.
		@author		Kevin White
		@date		20 Dec 2020
	*/
void LpInstruction::setLpi(const char* lpi) {
	this->lpi = lpi;
}

/*!
	@brief		Resets the LPI by setting it to a nullptr.
	@author		Kevin White
	@date		20 Dec 2020
*/
void LpInstruction::reset() {
	Instruction::reset();
	setLpi(nullptr);
}

/*!
	@brief		Initialises this LP instruction from an existing
				LP instruction.
	@author		Kevin White
	@date		21 Dec 2020
*/
void LpInstruction::init(LpInstruction* lpInstruction) {
	if (lpInstruction == nullptr) {
		return;
	}

	setLpi(lpInstruction->getLpi());
	SetNumberOfSteps(lpInstruction->GetNumberOfSteps());
	SetDuration(lpInstruction->GetDuration());
}

/*!
	@brief		Determines whether there are further remaining
				animation steps for this instruction.
	@returns	True if there are more steps or false if there
				are no further steps.
	@author		Kevin White
	@date		27 Dec 2020
*/
bool LpInstruction::HasMoreSteps() {
	return remainingSteps > 0;
}

/*!
	@brief		Reduces the number of remaining animation
				steps by 1.
	@author		Kevin White
	@date		27 Dec 2020
*/
void LpInstruction::DecrementSteps() {
	remainingSteps--;
}

/*!
	@brief		Gets the number of animation steps remaining
				to complete the instruction.
	@returns	The remaining number of animation steps.
	@author		Kevin White
	@date		27 Dec 2020
*/
uint16_t LpInstruction::GetNumberOfSteps() {
	return remainingSteps;
}

/*!
	@brief		Gets the current animations step.
	@returns	The current animation step number.
	@author		Kevin White
	@date		14 Jan 2020
*/
uint16_t LpInstruction::GetCurrentStep() {
	return steps - remainingSteps;
}

/*!
	@brief		Sets the number of animation steps remaining
				to complete the instruction.
	@param		numberOfSteps	The number of remaining animations steps.
	@author		Kevin White
	@date		27 Dec 2020
*/
void LpInstruction::SetNumberOfSteps(uint16_t numberOfSteps) {
	steps = remainingSteps = numberOfSteps;
}

/*!
	@brief		Resets the duration back to the duration
				specified in the instruction.
	@author		Kevin White
	@date		27 Dec 2020
*/
void LpInstruction::ResetDuration() {
	currentDuration = duration;
}

/*!
	@brief		Resets the duration and number of steps
				back to those specified by the LPI.
	@author		Kevin White
	@date		27 Dec 2020
*/
void LpInstruction::ResetDurationAndSteps() {
	ResetDuration();
	SetNumberOfSteps(steps);
}

/*!
	@brief		Gets whether it is time to render the instruction.
				That occurs when the currentDuration is the same
				value as duration (i.e. the duration specified in the
				LPI string).
	@returns	True if it is time to render the instruction or
				false if it is not yet time to render.
	@author		Kevin White
	@date		27 Dec 2020
*/
bool LpInstruction::IsTimeToRender() {
	return currentDuration == duration;
}

/*!
	@brief		Decrements the current duration by 1 and returns
				the new value.
	@returns	The decremented value of current duration.
	@author		Kevin White
	@date		29 Dec 2020
*/
uint8_t LpInstruction::DecrementCurrentDuration() {
	return --currentDuration;
}

	/*!
		@brief		Sets the duration of each animation step.
		@param		duration		The duration of each animation step in rendering frames.
		@author		Kevin White
		@date		27 Dec 2020
	*/
	void LpInstruction::SetDuration(uint8_t duration) {
		this->currentDuration = this->duration = duration;
	}

	/*!
		@brief		Gets the duration of each animation step.
		@returns	The duration in rendering frames of each animation step.
		@author		Kevin White
		@date		27 Dec 2020
	*/
	uint8_t LpInstruction::GetDuration() {
		return duration;
	}
}