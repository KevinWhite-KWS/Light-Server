/*!
 * @file LPI.h
 *
 * Abstract base class for any LPI that is non-animated.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */


#ifndef _NONANIMATEDLPI_h
#define _NONANIMATEDLPI_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\WProgram.h"
#endif

#include "LPI.h"

namespace LS {
	/*!
	@brief  Abstract base class for any non-animated LPI.
	*/
	class NonAnimatedLPI : public LPI {
	public:
		/*!
		  @brief   Constructor injects the required dependencies and sets the number of steps to 1 as it's non-animated.
		  @param   lpiInstruction	 A pointer to the LPI instruction.
		  @param   ledConfig		 A pointer to the LEDConfig instance that contains the configuration information about the LEDs.
		  @param   stringProcessor	 A pointer to the StringProcessor instance that provides string processing functionality.
		*/
		NonAnimatedLPI(LPIInstruction* lpiInstruction, const LEDConfig* ledConfig, StringProcessor* stringProcessor)
			: LPI(lpiInstruction, ledConfig, stringProcessor) {
			this->totalSteps = 1;
		}
	};
}

#endif

