// 
// 
// 

#include "StringProcessor.h"

namespace LS {
	/*!
	  @brief   Extracts a hexidecimally encoded colour from a string.  The
			   string should dbe 6 bytes in the format: RRGGBB.  That is, 2-byte hexidecimal
			   for red, 2-byte hexidecimal for green, and 2-byte hexidecimal for blue.
	  @param   colourString  The pointer to the strin gthat contains the hexidecimally encoded colour.
	  @param   isValid		 A reference to the boolean type that will be set to true if the colour
							 is valid or false otherwiose.
	  @return  A Colour struct containing the red, green, and blue component values as uint8_t type.
	*/
	const Colour StringProcessor::ExtractColourFromHexEncoded(const char* colourString, bool& isValid) {
		isValid = false;
		Colour colour;

		if (colourString == nullptr) return colour;
		
		uint8_t currentColourComponent = 1;		// 0 = red, 1 = green, 2 = blue
		uint8_t* currentColour = &colour.red;
		bool isHigh = true;
		uint8_t charBase = 0;

		int pos = 0;
		char currentChar;
		while ((currentChar = *colourString++) != '\0' && pos++ < 6) {
			if (currentChar > 47 && currentChar < 58)  charBase = 48;		// 0 - 9
			else if (currentChar > 64 && currentChar < 71) charBase = 55;	// A - F
			else return colour;		// Invalid character

			// Set the high nibble of the current colour
			// if we are processing the high nibble.  Otherwise, set the
			// low nibble of the current colour
			if (isHigh) {
				*currentColour = (currentChar - charBase) << 4;
				isHigh = false;
			}
			else {
				*currentColour |= currentChar - charBase;
				isHigh = true;
				// Set the colour point to the next colour, either green
				// or blue.
				if (++currentColourComponent == 2) currentColour = &colour.green;
				else currentColour = &colour.blue;
			}
		}

		// set isValid depending on whether we have processed 6 character or not
		isValid = pos >= 6;

		return colour;
	}

	/*!
	  @brief   Extracts a hexidecimally encoded number from a string.  The
			   string should be 2 bytes in the format: NN.  That is, 2-byte hexidecimal
			   encoded vlaue is expected between minExpectedValue and maxExpectedValue.
	  @param   numberString  The pointer to the string that contains the hexidecimally encoded number.
	  @param   minExpected	 The minimum value that is expected.
	  @param   maxExpected	 The maximum value that is expected.
	  @param   isValid		 A reference to the boolean type that will be set to true if the
							 number is valid and between minExpected and maxExpected.  Otherwise,
							 it will be set to false.
	  @return  A uint8_t value that represents the value parsed from the hexidecimal string.
	*/
	const uint8_t StringProcessor::ExtractNumberFromHexEncoded(const char* numberString, uint8_t minExpectedValue, uint8_t maxExpectedValue, bool& isValid) {
		isValid = false;

		if (numberString == nullptr || minExpectedValue > maxExpectedValue) return 0;

		uint8_t extractedNumber = 0;
		uint8_t charBase = 0;

		int pos = 0;
		char currentChar;
		while ((currentChar = *numberString++) != '\0' && pos++ < 2) {
			if (currentChar > 47 && currentChar < 58)  charBase = 48;		// 0 - 9
			else if (currentChar > 64 && currentChar < 71) charBase = 55;	// A - F
			else return 0;

			if (pos == 1) extractedNumber = (currentChar - charBase) << 4;	// set the high nibble
			else extractedNumber |= currentChar - charBase;					// set the low nibble
		}

		// set isValid depending on whetehr we have processed 2 characters as expected
		isValid = (pos == 2 || pos == 3) && extractedNumber >= minExpectedValue && extractedNumber <= maxExpectedValue;

		return extractedNumber;
	}

	/*!
	  @brief   Extracts a Rendering Instruction (RI) value from a string.  A RI
			   consists of a hexidecimally encoded colour and a hexidecimally encoded number
			   which specifies how many LEDs are affected.  The string should thus be formatted:
			   RRGGBBNN (RR = red, GG = green, BB = blue, NN = number of LEDs affected (1-255)).
	  @param   riString		 The pointer to the string that contains the hexidecimally encoded RI.
	  @param   isValid		 A reference to the boolean type that will be set to true if the
							 number is valid and between minExpected and maxExpected.  Otherwise,
							 it will be set to false.
	  @return  A RI structure containing the parsed colour and number of affected LEDs.
	*/
	const RI StringProcessor::ExtractRIFromHexEncoded(const char* riString, bool& isValid) {
		isValid = false;
		RI ri;

		if (riString == nullptr) return ri;

		// attempt to extract the colour component
		const char* pRIString = riString;
		Colour ledColour = ExtractColourFromHexEncoded(pRIString, isValid);
		if (isValid == false) return ri;
		ri.colour = ledColour;

		// attempt to extract the number of LEDs affected
		pRIString += 6;
		uint8_t affectedLeds = ExtractNumberFromHexEncoded(pRIString, 1, 255, isValid);
		if (isValid == false) return ri;
		ri.number = affectedLeds;

		return ri;
	}

	/*!
	  @brief   Extracts a LPI instruction value from a string.  A valid LPI instruction
			   consists of four hex encoded parts: xx (op code), yy (duration), rr (reserved), zz (reserved).
	  @param   instructionString	 The pointer to the string that contains the hexidecimally encoded LPI instruction.
	  @param   isValid				 A reference to the boolean type that will be set to true if the string contains
									 a valid LPI instruction.  If it is not valid then the value is set to false.
	  @return  A LPIInstruction structure instance.
	*/
	const LPIInstruction StringProcessor::ExtractLPIFromHexEncoded(const char* instructionString, bool& isValid) {
		isValid = false;
		LPIInstruction lpi = LPIInstruction();
		
		// Extract the op-code (00-FF are valid)
		const char* pInstructionString = instructionString;
		uint8_t opCode = ExtractNumberFromHexEncoded(pInstructionString, 0, 255, isValid);
		if (isValid == false) return lpi;
		lpi.opcode = opCode;

		// Extract the duration (01-FF are valid)
		pInstructionString += 2;
		uint8_t duration = ExtractNumberFromHexEncoded(pInstructionString, 1, 255, isValid);
		if (isValid == false) return lpi;
		lpi.duration = duration;

		// Extract the two reserved values (to ensure that they have bene defined)
		pInstructionString += 2;
		ExtractNumberFromHexEncoded(pInstructionString, 0, 0, isValid);
		if (isValid == false) return lpi;
		pInstructionString += 2;
		ExtractNumberFromHexEncoded(pInstructionString, 0, 0, isValid);

		return lpi;
	}

	/*!
	  @brief   Extracts a simple boolean value from a hex encoded string.
	  @param   instructionString	 The pointer to the string that contains the hexidecimally encoded boolean.
	  @param   isValid				 A reference to the boolean type that will be set to true if the string contains
									 a valid boolean value.  If it is not valid then the value is set to false.
	  @return  A boolean which is false if the hex-encoded value is 0 or true if the hex-encoded value is 1.
	*/
	const bool StringProcessor::ExtractBoolFromHexEncoded(const char* boolString, bool& isValid) {
		// isValid = false;
		// if (boolString == nullptr) return false;

		isValid = boolString != nullptr && (*boolString == '0' || *boolString == '1');
		bool boolValue = isValid && *boolString == '1';

		return boolValue;
	}
}