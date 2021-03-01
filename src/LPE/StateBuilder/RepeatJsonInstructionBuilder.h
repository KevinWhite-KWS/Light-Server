#ifndef _RepeatJsonInstructionBuilder_h
#define _RepeatJsonInstructionBuilder_h

#include "..\..\ArduinoJson-v6.17.2.h"
#include "..\..\ValueDomainTypes.h"
#include "LpState.h"
#include "..\Instructions\RepeatInstruction.h"
#include "IJsonInstructionBuilder.h"

namespace LS {
	/*!
		@brief	Builds a repeat instruction from a JSON document.
		@author	Kevin White
		@date	23 Dec 2020
	*/
	class RepeatJsonInstructionBuilder : public IJsonInstructionBuilder {
	private:
		RepeatInstruction repeatInstruction;

	public:
		Instruction* BuildInstruction(JsonVariant* jsonVar, LpState* state);
	};
}

#endif