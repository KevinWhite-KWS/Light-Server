#ifndef _LpJsonValidator_h
#define _LpJsonValidator_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include "JsonInstructionValidatorFactory.h"

namespace LS {
	/*!
		@brief	Validates that an LPI instruction, from a JSON document, is well-formed
				and validate according to the rules of the specific LPI.
		@author	Kevin White
		@date	17 Dec 2020
	*/
	class LpJsonValidator {
		private:
			JsonInstructionValidatorFactory* validatorFactory;
			// 2000: *** BUFFER ALLOCATION *** - An entire LP program requiring validation
			StaticJsonDocument<BUFFER_LP_VALIDATION> validateJsonDoc;
			
			bool hasInfiniteLoop = false;

		protected:
			void ValidateInstructions(JsonArray* instructions, LPValidateResult* result);

		public:
			LpJsonValidator(JsonInstructionValidatorFactory* factory);

			virtual void ValidateLp(FixedSizeCharBuffer* lp, LPValidateResult* result);
	};
}

#endif
