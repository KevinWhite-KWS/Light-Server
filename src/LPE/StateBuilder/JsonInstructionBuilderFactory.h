#ifndef _JsonInstructionBuilerFactory_h
#define _JsonInstructionBuilerFactory_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include "IJsonInstructionBuilder.h"
#include "LpJsonInstructionBuilder.h"
#include "RepeatJsonInstructionBuilder.h"
#include "..\InstructionType.h"
#include "..\LpiExecutors\LpiExecutorFactory.h"

namespace LS {
	/*!
		@brief	Provides access to instances to build instructions in a Light Program.
		@author	Kevin White
		@date	23 Dec 2020
	*/
	class JsonInstructionBuilderFactory {
	private:
		IJsonInstructionBuilder* builders[2];

	public:
		JsonInstructionBuilderFactory(LpiExecutorFactory* lpiExecutorFactory, StringProcessor* stringProcessor, LEDConfig* ledConfig);
		~JsonInstructionBuilderFactory();

		IJsonInstructionBuilder* GetInstructionBuilder(InstructionType instructionType);
	};
}


#endif