/*!
 * @file FadeAnimatedLPI.h
 *
 * LPI instructions 04: Fade
 * Fades-in or fades-out the LEDS from a start colour to an end colour
 * in a specified increment.
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 */

#ifndef _FADEANIMATEDLPI_h
#define _FADEANIMATEDLPI_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\WProgram.h"
#endif

#include "AnimatedLPI.h"
#include <math.h>

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)

namespace LS {
	class FadeAnimatedLPI : public AnimatedLPI {
		protected:
			Colour startColour;
			Colour endColour;
			uint8_t step = 1;
			bool fadeOut = false;

			virtual bool const ValidateInstruction();
		public:
			/*!
			  @brief   Constructor injects the required dependencies.
			  @param   lpiInstruction	 A pointer to the LPI instruction.
			  @param   ledConfig		 A pointer to the LEDConfig instance that contains the configuration information about the LEDs.
			  @param   stringProcessor	 A pointer to the StringProcessor instance that provides string processing functionality.
			*/
			FadeAnimatedLPI(const LEDConfig* ledConfig, StringProcessor* stringProcessor)
				: AnimatedLPI(ledConfig, stringProcessor) {
				this->opCode = 4;
			}

			virtual bool FadeAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer);

			/*!
			  @brief   Gets the op-code of the LPI.
			  @returns The LPI op-code.
			*/
			const virtual uint8_t GetOpCode() {
				return 4;
			}
	};
}

#endif

