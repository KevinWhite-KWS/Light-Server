#ifndef _IJsonInstructionBuilder_h
#define _IJsonInstructionBuilder_h

#include "..\..\ArduinoJson-v6.17.2.h"
#include "..\..\ValueDomainTypes.h"
#include "LpState.h"
#include "..\Instructions\Instruction.h"

namespace LS {
	/*
	@brief	IJsonInstructionBuilder defines the contract for a class
			performs instruction building e.g. build repeat instruction.
	@author	Kevin White
	@date	12 Dec 2020
	*/
	class IJsonInstructionBuilder {
	public:
		virtual Instruction* BuildInstruction(JsonVariant* jsonVar, LpState* state) = 0;
	};
}

#endif