#include "JsonInstructionBuilderFactory.h"

namespace LS {
	/*!
		@brief	Constructor instantiates the instruction builders.
		@param	lpiFactory		A pointer to the factory that provides access to the LPI instructions.
		@param	stringProcessor	A pointer to the class that provides string parsing.
		@param	ledConfig		A pointer to the class that contains configuration information about the LEDs.
		@author	Kevin White
		@date	23 Dec 2020
	*/
	JsonInstructionBuilderFactory::JsonInstructionBuilderFactory(LpiExecutorFactory* lpiExecutorFactory, StringProcessor* stringProcessor, LEDConfig* ledConfig) {
		builders[InstructionType::Lpi] = new LpJsonInstructionBuilder(lpiExecutorFactory, stringProcessor, ledConfig);
		builders[InstructionType::Repeat] = new RepeatJsonInstructionBuilder();
	}

	/*!
		@brief	Destructor frees up allocated memory.
		@author	Kevin White
		@date	23 Dec 2020
	*/
	JsonInstructionBuilderFactory::~JsonInstructionBuilderFactory() {
		free(builders[InstructionType::Lpi]);
		free(builders[InstructionType::Repeat]);
	}

	/*!
		@brief	Factory method gets an instruction builder instance.
		@param	builderType		The type of instruction builder to return.
		@returns	A pointer to the required instruction builder or null if the builder does not exist.
		@author	Kevin White
		@date	23 Dec 2020
	*/
	IJsonInstructionBuilder* JsonInstructionBuilderFactory::GetInstructionBuilder(InstructionType instructionType) {
		switch (instructionType) {
			case InstructionType::Lpi:
			case InstructionType::Repeat:
				return builders[instructionType];
			}

		return nullptr;
	}
}