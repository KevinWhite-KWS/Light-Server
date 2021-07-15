#ifndef _LpJsonInstructionBuilder_h
#define _LpJsonInstructionBuilder_h

#include "..\..\ArduinoJson-v6.17.2.h"
#include "..\..\ValueDomainTypes.h"
#include "LpState.h"
#include "..\Instructions\LpInstruction.h"
#include "IJsonInstructionBuilder.h"
#include "..\LpiExecutors\LpiExecutorFactory.h"
#include "..\..\StringProcessor.h"

namespace LS {
	/*!
		@brief	Builds an LP instruction from a JSON document.
		@author	Kevin White
		@date	23 Dec 2020
	*/
	class LpJsonInstructionBuilder : public IJsonInstructionBuilder {
	private:
		LpInstruction lpInstruction;
		
		
		LEDConfig* ledConfig;
		StringProcessor* stringProcessor;
		LpiExecutorFactory* lpiFactory;
		LpiExecutorParams lpiExecutorParams;

		// 500:  *** BUFFER ALLOCATION *** - Individual LPI building
		//// DON'T KNOW WHAT THS WAS FOR - TO BE REMOVED
		FixedSizeCharBuffer lpiBuffer = FixedSizeCharBuffer(BUFFER_LPI_VALIDATION);


		LPIInstruction lpiBasics;
	public:
		LpJsonInstructionBuilder(LpiExecutorFactory* lpiInstructionFactory, StringProcessor* stringBuilder, LEDConfig* ledConfig);

		Instruction* BuildInstruction(JsonVariant* jsonVar, LpState* state);
	};
}

#endif