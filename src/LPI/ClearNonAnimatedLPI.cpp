#include "ClearNonAnimatedLPI.h"

namespace LS {

	/*!
	  @brief   Fills the RI buffer with the next RI to be executed.  If there
			   is no RI to be executed next or the instruction is not in a valid
			   state then false is returned.
	  @returns True if the next RI buffer was correctly filled.  False otherwise.
	*/
	bool ClearNonAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer) {
		if (this->instructionIsValid == false) return false;

		// Fill the buffer
		riBuffer->LoadFromBuffer(nextRI);

		return true;
	}

	/*!
	  @brief   Validates the instruction according to the validation rules
			   for this type of LPI.  Always returns true as the base validation
			   already does enough.
	  @returns True if the instruction is valid or false it is not.
	*/
	const bool ClearNonAnimatedLPI::ValidateInstruction() {
		return true;
	}

	///*!
	//  @brief  Resets the state of the 
	//*/
	//bool ClearNonAnimatedLPI::Reset(FixedSizeCharBuffer* instructionBuffer) {
	//	return false;
	//}
}


