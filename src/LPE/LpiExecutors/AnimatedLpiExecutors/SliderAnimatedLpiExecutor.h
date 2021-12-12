#ifndef _SliderAnimatedLpiExecutor_h
#define _SliderAnimatedLpiExecutor_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\..\WProgram.h"
#endif

#include "AnimatedLpiExecutor.h"
#include "..\..\..\ValueDomainTypes.h"
#include "..\LpiExecutorParams.h"
#include "..\LpiExecutor.h"
#include "..\..\EffectHelpers\GradientEffect.h"

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)

namespace LS {
	/*!
		@brief		Provides an executor implementation for the slider LPI.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	class SliderAnimatedLpiExecutor : public AnimatedLpiExecutor {
	private:
		GradientEffect gradientEffect;

	protected:
		void RenderTail(
			uint16_t& numLedsBeforeSlider,
			LpiExecutorParams* lpiExecParams,
			LpiExecutorOutput* output,
			uint8_t& sliderWidth,
			uint8_t& tailLength,
			Colour& backgroundColour,
			Colour& sliderColour
		);

		void RenderHead(
			uint16_t& numLedsAfterSlider,
			LpiExecutorParams* lpiExecParams,
			LpiExecutorOutput* output,
			uint8_t& sliderWidth,
			uint8_t& headLength,
			Colour& backgroundColour,
			Colour& sliderColour
		);

	public:
		virtual bool ValidateLpi(LpiExecutorParams* lpiExecParams);
		virtual uint16_t GetNumberOfSteps(LpiExecutorParams* lpiExecParams);
		virtual void Execute(LpiExecutorParams* lpiExecParams, uint16_t step, LpiExecutorOutput* output);
	};
}

#endif
