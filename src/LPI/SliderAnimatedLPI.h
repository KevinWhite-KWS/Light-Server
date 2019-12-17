/*!
 * @file SliderAnimatedLPI.h
 *
 * LPI instructions 03: Slider
 * Provides a slider effect, like the light on the car
 * from knight rider.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 */

#ifndef _SLIDERANIMATEDLPI_h
#define _SLIDERANIMATEDLPI_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\WProgram.h"
#endif

#include "AnimatedLPI.h"
#include "string.h"

namespace LS {
	class SliderAnimatedLPI : public AnimatedLPI {
		protected:
			bool startFar = false;
			uint8_t sliderWidth = 0;

			virtual bool const ValidateInstruction();
		public:
			/*!
			  @brief   Constructor injects the required dependencies.
			  @param   lpiInstruction	 A pointer to the LPI instruction.
			  @param   ledConfig		 A pointer to the LEDConfig instance that contains the configuration information about the LEDs.
			  @param   stringProcessor	 A pointer to the StringProcessor instance that provides string processing functionality.
			*/
			SliderAnimatedLPI(const LEDConfig* ledConfig, StringProcessor* stringProcessor)
				: AnimatedLPI(ledConfig, stringProcessor) {
				this->opCode = 3;
			}

			// virtual bool SliderAnimatedLPI::GetNextRI(FixedSizeCharBuffer* riBuffer);
			virtual bool GetNextRI(FixedSizeCharBuffer* riBuffer);

			/*!
			  @brief   Gets the op-code of the LPI.
			  @returns The LPI op-code.
			*/
			const virtual uint8_t GetOpCode() {
				return 3;
			}
	};
}

#endif

