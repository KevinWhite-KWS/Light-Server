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

#ifndef _RENDERER_h
#define _RENDERER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// #include "ValueDomainTypes.h"
#include "StringProcessor.h"
#include "DomainInterfaces.h"

namespace LS {
	/*!
	@brief  Class that renders the attached LEDs according to the given RI.
	*/
	class Renderer {
		protected:
			StringProcessor* stringProcessor = nullptr;
			const LEDConfig* ledConfig = nullptr;
			IPixelController* pixelController = nullptr;
			bool lastSetRiValid = false;

		public:
			/*!
			  @brief   Constructor injects the required dependencies.
			  @param   strProcessor		 A pointer to the StringProcessor instance that provides string parsing functionality.
			*/
			Renderer(StringProcessor* stringProcessor, const LEDConfig* ledConfig, IPixelController* pixelController) {
				// if (stringProcessor == nullptr) throw "strProcessor must not be a null pointer";
				// if (ledConfig == nullptr) throw "ledConfig must not be a null pointer";

				this->stringProcessor = stringProcessor;
				this->SetLEDConfig(ledConfig);
				this->pixelController = pixelController;
			}

			bool SetRI(const char* riString);

			void SetLEDConfig(const LEDConfig* ledConfig);

			void Render();

			bool AreAnyLEDsOn();
	};
}
#endif