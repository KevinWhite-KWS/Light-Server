#ifndef _StochasticNonAnimatedLpiExecutor_h
#define _StochasticNonAnimatedLpiExecutor_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\..\WProgram.h"
#endif

#include "NonAnimatedLpiExecutor.h"
#include "..\..\..\ValueDomainTypes.h"
#include "..\LpiExecutorParams.h"
#include "..\LpiExecutor.h"
#include <stdlib.h>

namespace LS {
	/*!
		@brief		Provides an executor implementation for the stochastic LPI.
		@author		Kevin White
		@date		8 Jan 2021
	*/
	class StochasticNonAnimatedLpiExecutor : public NonAnimatedLpiExecutor {
	public:
		virtual bool ValidateLpi(LpiExecutorParams* lpiExecParams);
		virtual void ExecuteNonAnimated(LpiExecutorParams* lpiExecParams, LpiExecutorOutput* output);
	};
}

#endif
