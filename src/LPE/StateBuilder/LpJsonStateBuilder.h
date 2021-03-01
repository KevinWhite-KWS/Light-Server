#ifndef _LpJsonStateBuilder_h
#define _LpJsonStateBuilder_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include "JsonInstructionBuilderFactory.h"
#include "LpJsonState.h"

namespace LS {
	/*!
		@brief	Builds a tree of instructions that represents the values
				to be executed that form a Light Program.  Building a tree
				allows us to easily navigate from one instruction to the next.
		@author	Kevin White
		@date	23 Dec 2020
	*/
	class LpJsonStateBuilder {
	private:
		JsonInstructionBuilderFactory* instructionFactory;

	protected:
		void BuildInstructions(JsonArray* instructions, LpJsonState* state, InstructionWithChild* parentInstruction);
	public:
		LpJsonStateBuilder(JsonInstructionBuilderFactory* instructionFactory);

		virtual bool BuildState(FixedSizeCharBuffer* lp, LpJsonState* state);
	};
}

#endif