#ifndef _Instruction_h
#define _Instruction_h

#include "..\InstructionType.h"

namespace LS {
	/*!
		@brief		Stores the basic details of a single instruction in a
					Light Program.  This basic details consist of pointers
					to the next instruction to be executed in the program
					and the parnet instruction.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	class Instruction {
		private:
			Instruction* parent;
			Instruction* next;
		public:
			void reset();
			void setNext(Instruction* sibling);
			Instruction* getNext();
			bool HasNext();
			void setParent(Instruction* parent);
			Instruction* getParent();
			bool HasParent();

			virtual InstructionType getInstructionType() = 0;
	};
}
#endif
