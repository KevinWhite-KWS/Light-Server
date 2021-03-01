#ifndef _IInstructionValidator_h
#define _IInstructionValidator_h

#include "..\..\ArduinoJson-v6.17.2.h"
#include "..\..\ValueDomainTypes.h"

namespace LS {
	/*
	@brief	IInstructionValidator defines the contract for a class that validates
			part of a LP.
	@author	Kevin White
	@date	12 Dec 2020
	*/
	class IJsonInstructionValidator {
		public:
			virtual void Validate(JsonVariant* jsonVar, LPValidateResult* result) = 0;
	};
}

#endif