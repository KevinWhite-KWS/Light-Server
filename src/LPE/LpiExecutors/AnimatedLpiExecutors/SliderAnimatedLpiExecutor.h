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

namespace LS {
	/*!
		@brief		Provides an executor implementation for the slider LPI.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	class SliderAnimatedLpiExecutor : public AnimatedLpiExecutor {
	public:
		virtual bool ValidateLpi(LpiExecutorParams* lpiExecParams);
		virtual uint16_t GetNumberOfSteps(LpiExecutorParams* lpiExecParams);
		virtual void Execute(LpiExecutorParams* lpiExecParams, uint16_t step, LpiExecutorOutput* output);
	};
}

#endif
