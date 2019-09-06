// StringProcessor.h


#ifndef _STRINGPROCESSOR_h
#define _STRINGPROCESSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ValueDomainTypes.h"

namespace LS {

	/*!
	@brief  Class that provides many string processor functions such
			as extracting a colour.
	*/
	class StringProcessor {
		public:

			const Colour ExtractColourFromHexEncoded(const char* colourString, bool& isValid);

			const uint8_t ExtractNumberFromHexEncoded(const char* numberString, uint8_t minExpectedValue, uint8_t maxExpectedValue, bool& isValid);

			const RI ExtractRIFromHexEncoded(const char* riString, bool& isValid);

			const LPIInstruction ExtractLPIFromHexEncoded(const char* instructionString, bool& isValid);

			const bool ExtractBoolFromHexEncoded(const char* instructionString, bool& isValid);

			const void ConvertNumberToHexEncoded(char* putBuffer, uint8_t number);
		};

}
#endif

