#include "LpJsonInstructionBuilder.h"

namespace LS {
	/*!
		@brief		Constructor sets the mandatory dependencies.
		@param		lpiFactory		Factory class that provides access to the
									LP instruction instances.
		@param		stringProcessor	A pointer to the class that provides string parsing.
		@param		ledConfig		A pointer to the class that contains configuration information about the LEDs.
		@author		Kevin White
		@date		27 Dec 2020
	*/
	LpJsonInstructionBuilder::LpJsonInstructionBuilder(LpiExecutorFactory* lpiFactory, StringProcessor* stringProcessor, LEDConfig* ledConfig) {
		this->lpiFactory = lpiFactory;
		this->stringProcessor = stringProcessor;
		this->ledConfig = ledConfig;
		lpiExecutorParams.Reset(&lpiBuffer, ledConfig, stringProcessor);
	}

	/*!
		@brief	Adds an lp instruction to the program that
				is taken from a JSON input document.
				NOTE: we do not verify the repeat statement here, it
				must already have been verified.
		@param	jsonVar		The JSON variant that represents the LP instruction.
		@param	state		The Light Program state.
		@author	Kevin White
		@date	23 Dec 2020
	*/
	Instruction* LpJsonInstructionBuilder::BuildInstruction(JsonVariant* jsonVar, LpState* state) {
		if (jsonVar == nullptr || state == nullptr) {
			return nullptr;
		}

		// get the LPI string
		const char* lpi = jsonVar->as<const char*>();

		lpInstruction.reset();

		// get the basic LPI details including the duration
		stringProcessor->ExtractLPIFromHexEncoded(lpi, &lpiBasics);

		// get the LPI executor so we can the number of steps
		// to complete the LPI
		lpiBuffer.LoadFromBuffer(lpi);
		LpiExecutor* lpiExecutor = lpiFactory->GetLpiExecutor(lpiBasics.opcode);
		uint16_t steps = lpiExecutor->GetNumberOfSteps(&lpiExecutorParams);

		lpInstruction.SetDuration(lpiBasics.duration);
		lpInstruction.SetNumberOfSteps(steps);

		// add the repeat to Light Program tree
		lpInstruction.setLpi(lpi);
		Instruction* lpIns = state->addInstruction(&lpInstruction);

		return lpIns;
	}
}