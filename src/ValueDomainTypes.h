#ifndef _VALUEDOMAINTYPES_H
#define _VALUEDOMAINTYPES_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdio.h>
#include "FixedSizeCharBuffer.h"

namespace LS {
	/* Buffer allocation sizes */
	#define BUFFER_LPI_LOADING			400	 // 500		// buffer size for loading an individual LPI
	#define	BUFFER_LPI_VALIDATION		400	 // 500		// buffer size for validating an individual LPI
	// 3500 = CRASH @ 233 LEDS
	#define BUFFER_LP_VALIDATION		3500 // 4000		// buffer size for validating an entire LP
	#define	BUFFER_LP					3500 // 4000		// buffer size for an executing LP


	#define	BUFFER_JSON_RESPONSE_SIZE	150	 // 200

	//#define BUFFER_LPI_LOADING			1000		// buffer size for loading an individual LPI
	//#define	BUFFER_LPI_VALIDATION		1000		// buffer size for validating an individual LPI
	//#define BUFFER_LP_VALIDATION		5000		// buffer size for validating an entire LP
	//#define	BUFFER_LP					5000		// buffer size for an executing LP

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

		/*!
			@brief		Sets this colour from another colour instance.
			@param		colour		The colour instance from which to set the values.
			@author		Kevin White
			@date		2 Jan 2021
		*/
		void SetFromColour(Colour* colour) {
			red = colour->red;
			green = colour->green;
			blue = colour->blue;
		}

		/*!
			@brief		Compares this colour with another colour and determines whether
						they are the same colour.
			@returns	True if the two colours are the same or false if they are different.
			@author		Kevin White
			@date		3 Jan 2021
		*/
		bool operator==(const Colour& otherColour) const {
			return this->red == otherColour.red
				&& this->green == otherColour.green
				&& this->blue == otherColour.blue;
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

		/*!
			@brief		Compares this RI with another RI in order to determine whether
						they are the same colour and same number of LEDs.
			@returns	True if the two RIs are the same or false if they are different in any way.
			@author		Kevin White
			@date		3 Jan 2021
		*/
		bool operator==(const RI& otherRi) const {
			return this->number == otherRi.number
				&& this->colour == otherRi.colour;
		}

		/*!
			@brief		Compares this RI with another RI in order to determine whether
						they are the same colour and same number of LEDs.
			@returns	True if the two RIs are the different, false if they are the same.
			@author		Kevin White
			@date		3 Jan 2021
		*/
		bool operator!=(const RI& otherRi) const {
			return !(*this == otherRi);
		}
	};

	/*!
	@brief  Struct that represents the configuration of the LEDs.  This includes: the number
			of pixels available (LEDs).
	*/
	struct LEDConfig {
		uint8_t controlValue = 99;			// value ONLY used to verify that values retrived from storage are valid
		uint16_t numberOfLEDs;

		//LEDConfig(uint8_t numberOfLEDs) {
		//	this->numberOfLEDs = numberOfLEDs;
		//}

		//LEDConfig() {
		//}

		bool AreSettingsValid() {
			return controlValue == 99;
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
		LoopHasInvalidTimesValue = 10
	};

	/*!
	  @brief	LPValidateResult encapsulates the result of validating a Light Program.
				The result consists of a validation result code and, potentially,
				additional information such as the name of a mandatory parameter.
	*/
	class LPValidateResult {
		protected:
			LPValidateCode code;
			// 100:  ***BUFFER ALLOCATION * **-Web response info buffer
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
				sprintf(buffer, "%d", result);					
				// sprintf_s(buffer, 100, "%d", result);		// Arduino doesn't like this?
			}
	};
}
	
#endif