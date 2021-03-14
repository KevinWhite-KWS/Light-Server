#ifndef _GradientEffect_h
#define _GradientEffect_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include "..\..\ValueDomainTypes.h"
#include <math.h>

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)

namespace LS {
	/*!
		@brief		Calculates the pixel colours required in a gradient
					between two colours.
		@author		Kevin White
		@date		9 March 2021
	*/
	class GradientEffect  {
	protected:
		Colour startColour;
		Colour endColour;
		uint8_t steps;

		// storage for values required to calculate the effect
		uint8_t absDistances[3];
		int stepDirections[3];
		double stepValues[3];
	public:
		/*!
			@brief		Constructor initialises member values to safe reasonable values.
			@author		Kevin White
			@date		10 March 2021
		*/
		GradientEffect() {
			for (int i = 0; i < 3; i++) {
				absDistances[i] = 0;
				stepDirections[i] = 1;
				stepValues[i] = 0;
			}
			steps = 1;
		}

		void Reset(Colour& startColour, Colour& endColour, uint8_t steps);

		void CalculatetepColour(uint8_t step, Colour& newColour);
	};
}

#endif
