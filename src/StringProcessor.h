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

			virtual const Colour ExtractColourFromHexEncoded(const char* colourString, bool& isValid);

			virtual const uint8_t ExtractNumberFromHexEncoded(const char* numberString, uint8_t minExpectedValue, uint16_t maxExpectedValue, bool& isValid);

			virtual const uint32_t ExtractNumberFromHex(const char* numberString, uint8_t minExpectedValue, uint16_t maxExpectedValue, bool& isValid);

			virtual const RI ExtractRIFromHexEncoded(const char* riString, bool& isValid);

			virtual const LPIInstruction ExtractLPIFromHexEncoded(const char* instructionString, bool& isValid);

			virtual bool ExtractLPIFromHexEncoded(const char* instructionString, LPIInstruction* lpInstruction);

			virtual const bool ExtractBoolFromHexEncoded(const char* instructionString, bool& isValid);

			virtual const void ConvertNumberToHexEncoded(char* putBuffer, uint8_t number);
		};

}
#endif

