#include "LPE.h"

namespace LS{
	LPI* LPE::GetLPI(FixedSizeCharBuffer* instructionBuffer) {
		if (instructionBuffer == nullptr) return nullptr;

		LPI* lpi = nullptr;

		// Extract the command parameters so we can detemrine
		// which LPI to execute
		char* pLpi = instructionBuffer->GetBuffer();
		bool isValid = false;
		LPIInstruction lpiInstruction = this->stringProcessor->ExtractLPIFromHexEncoded(pLpi, isValid);
		if (isValid == false) return nullptr;

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

	bool LPE::GetNextRI(FixedSizeCharBuffer* riBuffer) {
		return false;
	}
}