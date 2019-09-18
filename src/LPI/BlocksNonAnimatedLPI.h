/*!
 * @file ClearNonAnimatedLPI.h
 *
 * LPI instructions 00: Clear
 * Clears all LEDs to black, effectively turing them off.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _BLOCKSNONANIMATEDLPI_h
#define _BLOCKSNONANIMATEDLPI_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\WProgram.h"
#endif

#include "NonAnimatedLPI.h"
#include <math.h>

namespace LS {
	class BlocksNonAnimatedLPI : public NonAnimatedLPI {
		protected:
			uint8_t noOfBlocks = 0;

			virtual bool const ValidateInstruction();
		public:
			/*!
			  @brief   Constructor injects the required dependencies and sets the number of steps to 1 as it's non-animated.
			  @param   lpiInstruction	 A pointer to the LPI instruction.
			  @param   ledConfig		 A pointer to the LEDConfig instance that contains the configuration information about the LEDs.
			  @param   stringProcessor	 A pointer to the StringProcessor instance that provides string processing functionality.
			*/
			BlocksNonAnimatedLPI(LPIInstruction* lpiInstruction, const LEDConfig* ledConfig, StringProcessor* stringProcessor)
				: NonAnimatedLPI(lpiInstruction, ledConfig, stringProcessor) {
				this->opCode = 6;
			}

			virtual bool BlocksNonAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer);

			/*!
			  @brief   Gets the op-code of the LPI.
			  @returns The LPI op-code.
			*/
			const virtual uint8_t GetOpCode() {
				return 6;
			}
	};
}

#endif

