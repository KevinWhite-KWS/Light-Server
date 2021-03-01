#ifndef _LpiJsonInstructionValidator_h
#define _LpiJsonInstructionValidator_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include "IJsonInstructionValidator.h"
// #include "../../LPI/LPIFactory.h"
#include "../../FixedSizeCharBuffer.h"
#include "../../ValueDomainTypes.h"

#include "../LpiExecutors/LpiExecutorFactory.h"

namespace LS {
	/*!
		@brief	Validates that an LPI instruction, from a JSON document, is well-formed
				and validate according to the rules of the specific LPI.
		@author	Kevin White
		@date	17 Dec 2020
	*/
	class LpiJsonInstructionValidator : public IJsonInstructionValidator {
		private:
			// LPIFactory* lpiFactory;
			LpiExecutorFactory* lpiExecutorFactory;
			StringProcessor* stringProcessor;
			LEDConfig* ledConfig;
			LpiExecutorParams lpiExecutorParams;

			/* BUFFER ALLOCATED */
			FixedSizeCharBuffer lpiToBeValidatedBuffer = FixedSizeCharBuffer(BUFFER_LPI_VALIDATION);
			LPIInstruction lpiToBeValidated;
		public:
			// LpiJsonInstructionValidator(LPIFactory* factory);
			LpiJsonInstructionValidator(LpiExecutorFactory* factory, StringProcessor* stringProcessor, LEDConfig* ledConfig);

			void Validate(JsonVariant* jsonVar, LPValidateResult* result);
	};
}

#endif