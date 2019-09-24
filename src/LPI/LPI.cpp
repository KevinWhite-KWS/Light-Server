#include "LPI.h"

namespace LS {
	/*!
	  @brief   Constructor injects the required dependencies.
	  @param   lpiInstruction	 A pointer to the LPI instruction.
	  @param   ledConfig		 A pointer to the LEDConfig instance that contains the configuration information about the LEDs.
	*/
	LPI::LPI(const LEDConfig* ledConfig, StringProcessor* stringProcessor) {
		// if (instruction == nullptr) throw "instruction must not be a null pointer";
		// if (ledConfig == nullptr) throw "ledConfig must not be a null pointer";
		this->ledConfig = ledConfig;
		this->stringProcessor = stringProcessor;
	}

	/*!
	  @brief	Validtes the LPI instruction: ensures that it is not a nullptr and performs specific validation for the concrete LPI.
	  @returns	True if the LPI instruction is valid or false otherwise.
	*/
	bool LPI::Validate() {
		if (this->lpiInstruction == nullptr
			|| this->lpiInstruction->lpi == nullptr
			|| this->lpiInstruction->lpi->GetBuffer() == nullptr
			|| this->lpiInstruction->opcode != this->opCode
			|| this->lpiInstruction->duration == 0) {
			this->instructionIsValid = false;
			return false;
		}

		this->instructionIsValid = this->ValidateInstruction();
		return this->instructionIsValid;
	}

	/*!
	  @brief	Gets the total number of steps that are required as part of an animated LPI.  Non-animated LPIs will always return 1.
	  @returns	The number of steps involved for animated LPIs.
	*/
	uint16_t LPI::GetTotalNumberOfSteps() {
		return totalSteps;
	}
}


