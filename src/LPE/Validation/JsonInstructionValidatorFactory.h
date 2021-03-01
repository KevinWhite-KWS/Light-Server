#ifndef _JsonInstructionValidatorFactory_h
#define _JsonInstructionValidatorFactory_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include "IJsonInstructionValidator.h"
#include "LpiJsonInstructionValidator.h"
#include "RepeatJsonInstructionValidator.h"
#include "..\InstructionType.h"

#include "..\LpiExecutors\LpiExecutorFactory.h"

namespace LS {
	/*!
		@brief	Validates that an LPI instruction, from a JSON document, is well-formed
				and validate according to the rules of the specific LPI.
		@author	Kevin White
		@date	17 Dec 2020
	*/
	class JsonInstructionValidatorFactory {
		private:
			IJsonInstructionValidator* validators[2];

		public:
			// JsonInstructionValidatorFactory(LPIFactory* lpiFactory);
			JsonInstructionValidatorFactory(LpiExecutorFactory* lpiExecutorFactory, StringProcessor* stringProcessor, LEDConfig* ledConfig);
			~JsonInstructionValidatorFactory();

			IJsonInstructionValidator* GetValidator(InstructionType instructionType);
	};
}


#endif