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

#ifndef _CLEARNONANIMATEDLPI_h
#define _CLEARNONANIMATEDLPI_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\WProgram.h"
#endif

#include "NonAnimatedLPI.h"
#include "string.h"

#define NEXT_RI_CLEARNONANIMATEDLPI		"00000001R"

namespace LS {
	class ClearNonAnimatedLPI : public NonAnimatedLPI {
		protected:
			const char* nextRI = "00000001R";

			virtual bool const ValidateInstruction();
		public:
			/*!
			  @brief   Constructor injects the required dependencies and sets the number of steps to 1 as it's non-animated.
			  @param   lpiInstruction	 A pointer to the LPI instruction.
			  @param   ledConfig		 A pointer to the LEDConfig instance that contains the configuration information about the LEDs.
			  @param   stringProcessor	 A pointer to the StringProcessor instance that provides string processing functionality.
			*/
			ClearNonAnimatedLPI(const LEDConfig* ledConfig, StringProcessor* stringProcessor)
				: NonAnimatedLPI(ledConfig, stringProcessor) {
				this->opCode = 0;
			}

			virtual bool ClearNonAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer);

			/*!
			  @brief   Gets the op-code of the LPI.
			  @returns The LPI op-code.
			*/
			const virtual uint8_t GetOpCode() {
				return 0;
			}
	};
}

#endif

