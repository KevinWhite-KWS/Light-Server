#ifndef _PatternNonAnimatedLpiExecutor_h
#define _PatternNonAnimatedLpiExecutor_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\..\WProgram.h"
#endif

#include "NonAnimatedLpiExecutor.h"
#include "..\..\..\ValueDomainTypes.h"
#include "..\LpiExecutorParams.h"
#include "..\LpiExecutor.h"

namespace LS {
	/*!
		@brief		Provides an executor implementation for the pattern LPI.
		@author		Kevin White
		@date		4 Jan 2021
	*/
	class PatternNonAnimatedLpiExecutor : public NonAnimatedLpiExecutor {
	public:
		virtual bool ValidateLpi(LpiExecutorParams* lpiExecParams);
		virtual void ExecuteNonAnimated(LpiExecutorParams* lpiExecParams, LpiExecutorOutput* output);
	};
}

#endif
