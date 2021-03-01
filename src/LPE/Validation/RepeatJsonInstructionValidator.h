#ifndef _RepeatJsonInstructionValidator_h
#define _RepeatJsonInstructionValidator_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include "IJsonInstructionValidator.h"

namespace LS {
	/*!
		@brief	Validates that a repeat instruction contained in a JSON
				document is valid according to the rules for a repeat
				statement.
		@author	Kevin White
		@date	17 Dec 2020
	*/
	class RepeatJsonInstructionValidator : public IJsonInstructionValidator {
		public:
			void Validate(JsonVariant* jsonVar, LPValidateResult* result);
	};
}

#endif
