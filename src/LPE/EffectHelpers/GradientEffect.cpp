#include "GradientEffect.h"

namespace LS {
	/*!
		@brief		Resets the state of the gradient effect helper, ready
					to calculate the gradient colours between two new colours
					for a certain number of steps.
		@param		startColour		The start colour from which the gradient effect begins.
		@param		endColour		The end colour at which the gradient effect ends.
		@param		steps			The number of steps between the start and end for
									the gradient effect.
		@author		Kevin White
		@date		10 March 2021
	*/
	void GradientEffect::Reset(Colour& startColour, Colour& endColour, uint8_t steps) {
		if (steps == 0) {
			return;
		}

		this->startColour = startColour;
		this->endColour = endColour;
		this->steps = steps;

		// 1. Calculate absolute distances for each pixel component
		absDistances[0] = abs(endColour.red - startColour.red);
		absDistances[1] = abs(endColour.green - startColour.green);
		absDistances[2] = abs(endColour.blue - startColour.blue);

		// 2. Calculate step directions
		stepDirections[0] = startColour.red <= endColour.red ? 1 : -1;
		stepDirections[1] = startColour.green <= endColour.green ? 1 : -1;
		stepDirections[2] = startColour.blue <= endColour.blue ? 1 : -1;


		// 3. Calculate each individual step value
		stepValues[0] = (double)absDistances[0] / ((steps + 1) * stepDirections[0]);
		stepValues[1] = (double)absDistances[1] / ((steps + 1) * stepDirections[1]);
		stepValues[2] = (double)absDistances[2] / ((steps + 1) * stepDirections[2]);
	}

	/*!
		@brief		Calculates the colour of a particular step of the gradient effect.
		@param		step		The step number for which a gradient colour is required.
		@param		newColour	A reference to the Colour instance that will store the calculated colour.
		@author		Kevin White
		@date		10 March 2021
	*/
	void GradientEffect::CalculatetepColour(uint8_t step, Colour& newColour) {
		if (step == 0 || step > steps) {
			newColour.red = newColour.green = newColour.blue = 0;
			return;
		}

		// calculate the new colours
		int newPixelColours[3] = {
			(startColour.red + step * stepValues[0]) + 0.5,
			(startColour.green + step * stepValues[1]) + 0.5,
			(startColour.blue + step * stepValues[2]) + 0.5
		};

		// adjust for boundary values
		newPixelColours[0] = stepDirections[0] == 1 ? min(endColour.red, newPixelColours[0]) : max(endColour.red, newPixelColours[0]);
		newPixelColours[1] = stepDirections[1] == 1 ? min(endColour.green, newPixelColours[1]) : max(endColour.green, newPixelColours[1]);
		newPixelColours[2] = stepDirections[2] == 1 ? min(endColour.blue, newPixelColours[2]) : max(endColour.blue, newPixelColours[2]);

		newColour.red = newPixelColours[0];
		newColour.green = newPixelColours[1];
		newColour.blue = newPixelColours[2];
	}
}