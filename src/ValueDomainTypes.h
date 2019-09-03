#ifndef _VALUEDOMAINTYPES_H
#define _VALUEDOMAINTYPES_H

#include <stdint.h>
#include "FixedSizeCharBuffer.h"

namespace LS {
	/*!
	@brief  Struct that represents a three component colour: red, green, and blue.
	*/
	struct Colour {
		uint8_t red;
		uint8_t green;
		uint8_t blue;

		// Default constructor sets the colour to black
		Colour() {
			red = green = blue = 0;
		}

		Colour(uint8_t red, uint8_t green, uint8_t blue) {
			this->red = red;
			this->green = green;
			this->blue = blue;
		}
	};

	/*!
	@brief  Struct that represents a single rendering instruction.  That is: a colour and the number
			of sequential LEDs to render as that colour.
	*/
	struct RI {
		Colour colour;
		uint8_t number;

		RI() {
			this->number = 1;
		}

		RI(Colour colourToUse, uint8_t numberToAffect) {
			colour = colourToUse;
			number = numberToAffect;
		}
	};

	/*!
	@brief  Struct that represents the configuration of the LEDs.  This includes: the number
			of pixels available (LEDs).
	*/
	struct LEDConfig {
		uint8_t numberOfLEDs;

		LEDConfig(uint8_t numberOfLEDs) {
			this->numberOfLEDs = numberOfLEDs;
		}
	};

	/*!
	@brief  Struct represents an individual Light Definition Instruction (LPI).
	*/
	struct LPIInstruction {
		uint8_t opcode;
		uint8_t duration;
		FixedSizeCharBuffer* lpi;

		/*!
		  @brief	Default constructor does nothing useful.
		*/
		LPIInstruction() {
			opcode = 0;
			duration = 0;
			lpi = nullptr;
		}

		/*!
		  @brief	Constructor sets the properties.
		  @param	opcode		The op-code of the LPI instruction.
		  @param	duration	The duration in rendering frame of the LPI.
		  @param	lpi			The pointer to the string LPI.
		*/
		LPIInstruction(uint8_t opcode, uint8_t duration, FixedSizeCharBuffer* lpi) {
			this->opcode = opcode;
			this->duration = duration;
			this->lpi = lpi;
		}

		/*!
		  @brief	Gets a pointer to the remainder of the LPI buffer once
			        it has been advanced over the 8-bytes of the LPI instruction
					(that is, advanced over the op-code, duration, xx reserved, and yy reserved).
					Using this method means that changes to the instruction format will
					mean only having to change it once here.
		  @returns	A poiunt to the advanced LPI buffer.
		*/
		char* GetLPIBuffer() {
			if (lpi == nullptr) return nullptr;
			char* lpiBuffer = lpi->GetBuffer();
			lpiBuffer += 8;
			return lpiBuffer;
		}
	};
}
	
#endif