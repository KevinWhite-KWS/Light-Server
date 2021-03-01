/*!
	@brief		Provides a class to interact with the hardware and
				activate LEDs.
	@author		Kevin White
	@date		16 Jan 21
*/
#ifndef _PixelRenderer_h
#define _PixelRenderer_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "../WProgram.h"
#endif

#include "../DomainInterfaces.h"
#include "../LPE/LpiExecutors/LpiExecutorOutput.h"

namespace LS {
	/*!
	@brief  Class that renders the attached LEDs according to the given RI.
	*/
	class PixelRenderer {
	protected:
		LEDConfig* ledConfig = nullptr;
		IPixelController* pixelController = nullptr;
		bool lastSetRiValid = false;

	public:
		PixelRenderer(IPixelController* pixelController, LEDConfig* ledConfig);

		virtual bool SetPixels(LpiExecutorOutput* lpiExecutorOutput);
		virtual void ShowPixels();
		virtual bool AreAnyPixelsOn();
	};
}
#endif