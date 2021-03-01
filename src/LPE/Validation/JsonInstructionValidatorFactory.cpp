#include "JsonInstructionValidatorFactory.h"

namespace LS {
	/*!
		@brief	Adds the available validators to the collection of validators.
		@param	lpiFactory		A reference to the class that gets access to individual LPIs.
		@param	stringProessor	A reference to the class that performs string parsing.
		@param	ledConfig		A reference to the class that contains configuration properties about the connected LEDs.
		@author	Kevin Whtite
		@date	18 Dec 2020
	*/
	JsonInstructionValidatorFactory::JsonInstructionValidatorFactory(LpiExecutorFactory* lpiFactory, StringProcessor* stringProcessor, LEDConfig* ledConfig) {
		validators[InstructionType::Lpi] = new LpiJsonInstructionValidator(lpiFactory, stringProcessor, ledConfig);
		validators[InstructionType::Repeat] = new RepeatJsonInstructionValidator();
	}

	/*!
		@brief	Destructor frees up allocated memory.
		@author	Kevin Whtite
		@date	18 Dec 2020
	*/
	JsonInstructionValidatorFactory::~JsonInstructionValidatorFactory() {
		free(validators[InstructionType::Lpi]);
		free(validators[InstructionType::Repeat]);
	}

	/*!
		@brief	Factory method gets an instruction validator instance.
		@param	validatorType	The type of instruction validator that is required.
		@author	Kevin Whtite
		@date	18 Dec 2020
	*/
	IJsonInstructionValidator* JsonInstructionValidatorFactory::GetValidator(InstructionType instructionType) {
		switch (instructionType) {
			case InstructionType::Lpi:
			case InstructionType::Repeat:
				return validators[instructionType];
		}

		return nullptr;
	}
}