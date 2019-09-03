/*!
 * @file LPI.h
 *
 * Abstract base class for any LPI that is animated.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */


#ifndef _ANIMATEDLPI_h
#define _ANIMATEDLPI_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\WProgram.h"
#endif

#include "LPI.h"

namespace LS {
	/*!
	@brief  Abstract base class for any animated LPI.
	*/
	class AnimatedLPI : public LPI {
	protected:
		uint16_t currentStep = 1;

	public:
		/*!
		  @brief   Constructor injects the required dependencies.
		  @param   lpiInstruction	 A pointer to the LPI instruction.
		  @param   ledConfig		 A pointer to the LEDConfig instance that contains the configuration information about the LEDs.
		  @param   stringProcessor	 A pointer to the StringProcessor instance that provides string processing functionality.
		*/
		AnimatedLPI(LPIInstruction* lpiInstruction, const LEDConfig* ledConfig, StringProcessor* stringProcessor)
			: LPI(lpiInstruction, ledConfig, stringProcessor) {
		}
	};
}

#endif

