#ifndef _InstructionWithChild_h
#define _InstructionWithChild_h

#include "Instruction.h"

namespace LS {
	/*!
		@brief		Stores the basic details of an instruction that
					has at least one child instruction.  For example:
					repeat instructions must have at least one instruction.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	class InstructionWithChild : public Instruction {
		private:
			Instruction* firstChild;
		public:
			void setFirstChild(Instruction* firstChild);
			Instruction* getFirstChild();
			void reset();
	};
}

#endif