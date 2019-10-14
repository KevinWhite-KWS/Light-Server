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
 */

#ifndef _LPE_h
#define _LPE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "../../Light Server/src/ArduinoJson-v6.12.0.h"

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

			LPI* GetLPI(FixedSizeCharBuffer* instructionBuffer);

			bool VerifyInstructions(JsonObject* instructionsObject, LPValidateResult* result);
			bool VerifyRepeat(JsonObject* repeatObjectg, LPValidateResult* result);
			bool VerifyInstruction(JsonVariant* instructionObject, LPValidateResult* result);

			uint8_t infiniteLoopCounter = 0;
			uint8_t nestedLoopCounter = 0;
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