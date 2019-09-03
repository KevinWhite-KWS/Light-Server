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
			LPI(LPIInstruction* lpiInstruction, const LEDConfig* ledConfig, StringProcessor* stringProcessor);

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
	};
}

#endif

