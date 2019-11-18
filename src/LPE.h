/*!
 * @file Renderer.h
 *
 * Processes RIs and renders attached LEDs.
 * 
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 * Navigating around the LP requires a more sophisticated approach.  We
 * need to know:
 * - which instruction is the current instruction in current instructions collection.
 *   That is, the 0 based index of the instruction in the instructions.
 * - which instructions element is the current instructions collection.  That is,
 *   the depth within the hierarchy.
 * - how many iterations are remaining for each embedded loop (max 5 embedded loops).
 *
 * This implies that we need:
 * - instructionsDepth: index of current instructions element (depth)
 * - instructionIndex[5]: 0 based index of current instruction at each level (the level is the index into the array)
 * - repeatIterationsRemaining[5]: 0 based index of remaing iterations for max 5 embedded loops
 *
 * The instructionsDepth value can be used to index instructionIndex and repeatIterationsRemaining
 * ! instructionsDepth also includes the very first instructions element, so index will be off by 1
 * ! 
 */

#ifndef _LPE_h
#define _LPE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "ArduinoJson-v6.12.0.h"
#else
	#include "WProgram.h"
	#include "../../Light Server/src/ArduinoJson-v6.12.0.h"
#endif



#include "StringProcessor.h"
#include "DomainInterfaces.h"
#include "LPI\LPI.h"
#include "LPI\ClearNonAnimatedLPI.h"
#include "LPI\SolidNonAnimatedLPI.h"
#include "LPI\FadeAnimatedLPI.h"
#include "LPI\SliderAnimatedLPI.h"
#include "LPI\FadeAnimatedLPI.h"
#include "LPI\PatternNonAnimatedLPI.h"
#include "LPI\StochasticNonAnimatedLPI.h"
#include "LPI\BlocksNonAnimatedLPI.h"

namespace LS {
	enum LPIType { 
		LPI_Clear,
		LPI_Solid,
		LPI_Pattern,
		LPI_Slider,
		LPI_Fade,
		LPI_Stochastic,
		LPI_Blocks
	};

	/*!
	@brief  Class that renders the attached LEDs according to the given RI.
	*/
	class LPE {
		protected:
			const LEDConfig* ledConfig;
			StringProcessor* stringProcessor;
			LPI* lpis[7];					
			StaticJsonDocument<1000> lpeDoc;		// Statically allocated memory buffer to store the deserialized LP
			FixedSizeCharBuffer lpi = FixedSizeCharBuffer(1000);

			LPIInstruction* currentLPIInstruction = new LPIInstruction();	// NEED TO FREE!

			bool lpValid = false;

			LPI* GetLPI(FixedSizeCharBuffer* instructionBuffer, bool validate = true);

			bool VerifyInstructions(JsonObject* instructionsObject, LPValidateResult* result);
			bool VerifyRepeat(JsonObject* repeatObjectg, LPValidateResult* result);
			bool VerifyInstruction(JsonVariant* instructionObject, LPValidateResult* result);

			uint8_t infiniteLoopCounter = 0;
			uint8_t nestedLoopCounter = 0;

			const char* PopToNextInstruction();


			/* Values required for executing the LP */
			const char* currentIns = nullptr;
			bool loadLpi = false;

			// Tracks number of iterations remaining in current repeat element.
			// This is -1 for an infinite loop.
			int loopIterationsRemaining = 0;
			int loopsRemaining[6] = { 0, 0, 0, 0, 0, 0 };
			int loopsLPIndices[6] = { 1, 0, 0, 0, 0, 0 };

			// Tracks the depth in the current LP that contains the
			// current set of "instructions" that are being processed.
			uint8_t instructionsDepth = 0;
			uint8_t repeatDepth = 0;

			// JsonObject* currentInstructions = nullptr;
			JsonObject::iterator currentIterator;
			// JsonVariant* currentInstruction = nullptr;
			LPI* currentLPI = nullptr;
			int currentInstructionRemainingPulses = 0;
			int pulseCounter = 0;

			void ProcessNextIteratorElement();
			void ProcessRepeatElement(JsonVariant* repeatElement);



		public:
			/*!
			  @brief   Constructor injects the required dependencies.
			  @param   ledConfig			 A pointer to the LEDConfig instance that provides the configuration of the LEDS.
			  @param   stringProcessor		 A pointer to the StringProcessor instance that provides string parsing functionality.
			*/
			LPE(const LEDConfig* ledConfig, StringProcessor* stringProcessor) {
				// if (stringProcessor == nullptr) throw "strProcessor must not be a null pointer";
				// if (ledConfig == nullptr) throw "ledConfig must not be a null pointer";
				this->stringProcessor = stringProcessor;
				this->ledConfig = ledConfig;

				// Initialise the collection of LPIs
				lpis[LPI_Clear] = new ClearNonAnimatedLPI(ledConfig, stringProcessor);
				lpis[LPI_Solid] = new SolidNonAnimatedLPI(ledConfig, stringProcessor);
				lpis[LPI_Pattern] = new PatternNonAnimatedLPI(ledConfig, stringProcessor);
				lpis[LPI_Slider] = new SliderAnimatedLPI(ledConfig, stringProcessor);
				lpis[LPI_Fade] = new FadeAnimatedLPI(ledConfig, stringProcessor);
				lpis[LPI_Stochastic] = new StochasticNonAnimatedLPI(ledConfig, stringProcessor);
				lpis[LPI_Blocks] = new BlocksNonAnimatedLPI(ledConfig, stringProcessor);
			}

			bool GetNextRI(FixedSizeCharBuffer* riBuffer);

			void ValidateLP(FixedSizeCharBuffer* lp, LPValidateResult* result);
	};
}
#endif