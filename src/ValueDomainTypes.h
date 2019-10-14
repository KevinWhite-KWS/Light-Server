#ifndef _VALUEDOMAINTYPES_H
#define _VALUEDOMAINTYPES_H

#include <stdint.h>
#include <stdio.h>
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

	/*!
	  @brief	LPValidateCode defines the states that can be returned after a Light Program
				has been validated.
	*/
	enum LPValidateCode {
		Valid = 1,
		ProgramTooBig = 2,
		OnlyOneInfiniteLoopAllowed = 3,
		MissingMandatoryProperties = 4,
		NoInstructionsInLoop = 5,
		InvalidInstruction = 6,
		NoIntructions = 7,
		InvalidProperty = 8,
		Maximum5NestedLoopsAllowed = 9,
		LoopHasInvalidTimesValue = 10,
	};

	/*!
	  @brief	LPValidateResult encapsulates the result of validating a Light Program.
				The result consists of a validation result code and, potentially,
				additional information such as the name of a mandatory parameter.
	*/
	class LPValidateResult {
		protected:
			LPValidateCode code;
			FixedSizeCharBuffer infoBuffer = FixedSizeCharBuffer(100);

		public:
			/*!
			  @brief	Resets the sresult code.
			  @param	code		The new LPValidateCode value
			*/
			void ResetResult(LPValidateCode code) {
				this->code = code;
			}

			/*!
			  @brief	Resets the state of the validation result.
			  @param	code		The new LPValidateCode value
			  @param	info		The new optional pointer to the string that contains additional information.
			*/
			void ResetResult(LPValidateCode code, const char* info) {
				this->code = code;
				this->infoBuffer.ClearBuffer();
				this->infoBuffer.LoadFromBuffer(info);
			}

			/*!
			  @brief	Gets the LPValidateCode result value.
			  @returns	The result value which is one of LPValidateCode.
			*/
			LPValidateCode GetCode() {
				return this->code;
			}

			/*!
			  @brief	Gets the additional information buffer for the result.  This may contain
						useful additional information such as a pointer to the buffer of an instruction
						that is not valid.
			  @returns	A pointer to the information buffer for the result.
			*/
			const char* GetInfo() {
				return this->infoBuffer.GetBuffer();
			}

			/*!
			  @brief	Sets the info buffer result to be an integer value.  The integer
						is converted into a string and loaded in the buffer.
			*/
			void SetInfoFromInt(int result) {
				this->infoBuffer.ClearBuffer();
				char* buffer = this->infoBuffer.GetBuffer();
				// sprintf(buffer, "%d", result);
				sprintf_s(buffer, 100, "%d", result);
			}
	};
}
	
#endif