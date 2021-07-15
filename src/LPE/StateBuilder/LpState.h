#ifndef _LpState_h
#define _LpState_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include <stdint.h>
#include "../Instructions/Instruction.h"
#include "../Instructions/LpInstruction.h"
#include "../Instructions/RepeatInstruction.h"

// #define MAX_LPINSTRUCTIONS		100
// #define MAX_REPEATINSTRUCTIONS	25

// xxxx: *** BUFFER ALLOCATION *** - Store a tree structure of a entire LP state
#define MAX_LPINSTRUCTIONS		65
#define MAX_REPEATINSTRUCTIONS	15


namespace LS {
	/*!
		@brief	Stores the data necessary to track the state of a Light Program
				whilst it is being executed.
		@author	Kevin White
		@date	21 Dec 2020
	*/
	class LpState {
		private:
			// allocate enough space to store 100 light program instructions
			LpInstruction lpInstructions[MAX_LPINSTRUCTIONS] = {};
			// allocate enough space to store 25 loop instructions
			RepeatInstruction repeatInstructions[MAX_REPEATINSTRUCTIONS] = {};

			// various pointers to instruction positions,
			// required to track instructions as the program executes
			Instruction* firstInstruction = nullptr;
			Instruction* currentInstruction = nullptr;

			// indexes to the instructions storage arrays
			uint8_t lpInstructionIndex = 0;
			uint8_t repeatIndex = 0;

		protected:
			Instruction* addRepeatInstruction(RepeatInstruction* repeatInstruction);
			Instruction* addLpInstruction(LpInstruction* lpInstruction);

		public:
			virtual void reset();
			virtual Instruction* getFirstInstruction();
			virtual Instruction* getCurrentInstruction();
			virtual void setCurrentInstruction(Instruction* currentInstruction);
			virtual Instruction* addInstruction(Instruction* newInstruction);
	};
}
#endif
