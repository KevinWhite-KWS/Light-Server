/*!
 * @file ClearNonAnimatedLPI.h
 *
 * LPI instructions 02: Pattern
 * Renders a pattern that repeats itself along the length of LEDs.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _PATTERNNONANIMATEDLPI_h
#define _PATTERNNONANIMATEDLPI_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\WProgram.h"
#endif

#include "NonAnimatedLPI.h"
#include "string.h"

namespace LS {
	class PatternNonAnimatedLPI : public NonAnimatedLPI {
		protected:
			uint8_t numberOfBlocks = 0;

			virtual bool const ValidateInstruction();
		public:
			/*!
			  @brief   Constructor injects the required dependencies and sets the number of steps to 1 as it's non-animated.
			  @param   lpiInstruction	 A pointer to the LPI instruction.
			  @param   ledConfig		 A pointer to the LEDConfig instance that contains the configuration information about the LEDs.
			  @param   stringProcessor	 A pointer to the StringProcessor instance that provides string processing functionality.
			*/
			PatternNonAnimatedLPI(const LEDConfig* ledConfig, StringProcessor* stringProcessor)
				: NonAnimatedLPI(ledConfig, stringProcessor) {
				this->opCode = 2;
			}

			virtual bool PatternNonAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer);

			/*!
			  @brief   Gets the op-code of the LPI.
			  @returns The LPI op-code.
			*/
			const virtual uint8_t GetOpCode() {
				return 2;
			}
	};
}

#endif

