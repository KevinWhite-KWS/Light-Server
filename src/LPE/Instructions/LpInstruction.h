#ifndef _LpInstruction_h
#define _LpInstruction_h

#include "instruction.h"
#include "LpInstruction.h"
#include <stdint.h>

namespace LS {
	/*!
		@brief		Stores an individual LPI instruction.  That is, an
					actual effect instruction that causes rendering
					on the LEDs.
		@author		Kevin White
		@date		20 Dec 2020
	*/
	class LpInstruction : public Instruction {
		private:
			const char* lpi;
			uint16_t steps;
			uint16_t remainingSteps;
			uint8_t duration;
			uint8_t currentDuration;

		public:
			const char* getLpi();
			void setLpi(const char* lpi);
			void reset();
			void init(LpInstruction* lpInstruction);

			// methods to handle the number of animation steps
			bool HasMoreSteps();
			void DecrementSteps();
			uint16_t GetNumberOfSteps();
			uint16_t GetCurrentStep();
			void SetNumberOfSteps(uint16_t numberOfSteps);

			// methods to handle the duration of the effect
			void ResetDuration();
			void ResetDurationAndSteps();
			bool IsTimeToRender();
			uint8_t DecrementCurrentDuration();
			void SetDuration(uint8_t duration);
			uint8_t GetDuration();

			/*!
				@brief		Gets the type of instruction.
				@author		Kevin White
				@date		21 Dec 2020
			*/
			InstructionType getInstructionType() {
				return InstructionType::Lpi;
			}
	};
}

#endif