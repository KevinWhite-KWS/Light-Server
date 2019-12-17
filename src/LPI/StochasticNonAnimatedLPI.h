/*!
 * @file StochasticNonAnimatedLPI.h
 *
 * LPI instructions 05: Stochastic
 * Outputs a set of random colours from a chosen limited set.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _STOCHASTICNONANIMATEDLPI_h
#define _STOCHASTICNONANIMATEDLPI_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\WProgram.h"
#endif

#include "NonAnimatedLPI.h"
#include <stdlib.h>

namespace LS {
	class StochasticNonAnimatedLPI : public NonAnimatedLPI {
		protected:
			uint8_t noOfElements = 0;

			virtual bool const ValidateInstruction();
		public:
			/*!
			  @brief   Constructor injects the required dependencies and sets the number of steps to 1 as it's non-animated.
			  @param   lpiInstruction	 A pointer to the LPI instruction.
			  @param   ledConfig		 A pointer to the LEDConfig instance that contains the configuration information about the LEDs.
			  @param   stringProcessor	 A pointer to the StringProcessor instance that provides string processing functionality.
			*/
			StochasticNonAnimatedLPI(const LEDConfig* ledConfig, StringProcessor* stringProcessor)
				: NonAnimatedLPI(ledConfig, stringProcessor) {
				this->opCode = 5;
			}

			// virtual bool StochasticNonAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer);
			virtual bool GetNextRI(FixedSizeCharBuffer* riBuffer);

			/*!
			  @brief   Gets the op-code of the LPI.
			  @returns The LPI op-code.
			*/
			const virtual uint8_t GetOpCode() {
				return 5;
			}
	};
}

#endif

