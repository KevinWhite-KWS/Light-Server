#ifndef _FadeAnimatedLpiExecutor_h
#define _FadeAnimatedLpiExecutor_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\..\WProgram.h"
#endif

#include "AnimatedLpiExecutor.h"
#include "..\..\..\ValueDomainTypes.h"
#include "..\LpiExecutorParams.h"
#include "..\LpiExecutor.h"
#include <math.h>

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)

namespace LS {
	/*!
		@brief		Provides an executor implementation for the fade LPI.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	class FadeAnimatedLpiExecutor : public AnimatedLpiExecutor {
	public:
		virtual bool ValidateLpi(LpiExecutorParams* lpiExecParams);
		virtual uint16_t GetNumberOfSteps(LpiExecutorParams* lpiExecParams);
		virtual void Execute(LpiExecutorParams* lpiExecParams, uint16_t step, LpiExecutorOutput* output);
	};
}

#endif
