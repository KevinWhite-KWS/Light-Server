#include "SolidNonAnimatedLPI.h"

namespace LS {

	/*!
	  @brief   Fills the RI buffer with the next RI to be executed.  If there
			   is no RI to be executed next or the instruction is not in a valid
			   state then false is returned.
	  @returns True if the next RI buffer was correctly filled.  False otherwise.
	*/
	bool SolidNonAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer) {
		if (this->instructionIsValid == false) return false;

		// Ensure the buffer is wide enough
		if (riBuffer->GetBufferSize() < 10) return false;

		// Fill the RI buffer with solid colour required
		// const char* pInstruction = this->lpiInstruction->lpi->GetBuffer() + 8;
		char* pInstruction = this->lpiInstruction->GetLPIBuffer();
		char* pRiBuffer = riBuffer->GetBuffer();
		for (int i = 0; i < 6; i++) *pRiBuffer++ = *pInstruction++;
		
		// Now, add '01' (affect one pixel) and 'R' (repeat along the entire length).  Finally,
		// terminate the string.
		*pRiBuffer++ = '0';
		*pRiBuffer++ = '1';
		*pRiBuffer++ = 'R';
		*pRiBuffer = '\0';

		return true;
	}

	/*!
	  @brief   Validates the instruction according to the validation rules
			   for this type of LPI.
	  @returns True if the instruction is valid or false it is not.
	*/
	const bool SolidNonAnimatedLPI::ValidateInstruction() {
		char* pInstruction = this->lpiInstruction->GetLPIBuffer();

		// Ensure a colour can be extract after the LPI
		Colour solidColour = stringProcessor->ExtractColourFromHexEncoded(pInstruction, this->instructionIsValid);

		return this->instructionIsValid;
	}
}


