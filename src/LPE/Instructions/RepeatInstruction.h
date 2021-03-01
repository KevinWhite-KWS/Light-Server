#ifndef _RepeatInstruction_h
#define _RepeatInstruction_h

#include "instructionWithChild.h"
#include "LpInstruction.h"

namespace LS {
	/*!
		@brief		Stores the details of a repeat instruction which
					includes: number of iterations specified by the repeat
					instruction and number of remaining iterations.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	class RepeatInstruction : public InstructionWithChild {
		private:
			int numberOfIterations = -1;
			int remainingIterations = -1;
		public:
			int getNumberOfIterations();
			void setNumberOfIterations(int numberOfIterations);
			int getRemainingIterations();
			void setRemainingIterations(int remainingIterations);
			void ResetRemainingIterations();
			int decrementRemainingIterations();
			bool isInfinite();

			void reset();
			void init(RepeatInstruction* repeatInstrution);

			/*!
				@brief		Gets the type of instruction.
				@author		Kevin White
				@date		21 Dec 2020
			*/
			InstructionType getInstructionType() {
				return InstructionType::Repeat;
			}
		};
}

#endif