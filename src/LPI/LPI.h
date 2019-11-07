/*!
 * @file LPI.h
 *
 * Abstract base class for any LPI.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _LPI_h
#define _LPI_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "..\WProgram.h"
#endif

#include "../ValueDomainTypes.h"
#include "../FixedSizeCharBuffer.h"
#include "../StringProcessor.h"
#include <string.h>

namespace LS {
	/*!
	@brief  Abstract base class for any LPI.
	*/
	class LPI {
		protected:
			LPIInstruction* lpiInstruction = nullptr;
			const LEDConfig* ledConfig;
			StringProcessor* stringProcessor;
			
			bool instructionIsValid = false;
			uint8_t opCode = 0;
			uint16_t totalSteps = 0;

			virtual const bool ValidateInstruction() = 0;
		
		public:
			LPI(const LEDConfig* ledConfig, StringProcessor* stringProcessor);

			bool Validate();

			uint16_t GetTotalNumberOfSteps();

			virtual bool GetNextRI(FixedSizeCharBuffer* riBuffer) = 0;

			/*!
			  @brief   Gets the op-code of the LPI.
			  @returns The LPI op-code.
			*/
			const static uint8_t GetOpCode() {
				return 0;
			};

			/*!
			  @brief	Resets the state of the LPI by loading in new a new LPI instruction
						and resetting variables, re-calcuating the number of steps (for animated
						LPIs).
			  @param	lpiInstruction	A point to the LPIInstruction instance that contains the details of the LPI.
			  @returns	bool			True if the LPI instruction is valid or false otherwise.
			*/
			bool Reset(LPIInstruction* lpiInstruction, bool validate = true) {
				if (lpiInstruction == nullptr) return false;

				this->lpiInstruction = lpiInstruction;

				if (validate == true) {
					return this->Validate();
				}
				else {
					return true;
				}
			}

			/*!
			  @brief	Resets the state of the LPI by loading in new a new LPI instruction
						and resetting variables, re-calcuating the number of steps (for animated
						LPIs).  This method does not validate the LPI.  The assumption here is
						that it has already been validated and does not need re-validating.
			  @param	lpiInstruction	A point to the LPIInstruction instance that contains the details of the LPI.
			*/
			void ResetNoValidate(LPIInstruction* lpiInstrction) {
				this->lpiInstruction = lpiInstrction;
			}
	};
}

#endif

