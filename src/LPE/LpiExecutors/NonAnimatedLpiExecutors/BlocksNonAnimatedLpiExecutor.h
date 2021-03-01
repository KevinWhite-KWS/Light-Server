#ifndef _BlocksNonAnimatedLpiExecutor_h
#define _BlocksNonAnimatedLpiExecutor_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\..\WProgram.h"
#endif

#include "NonAnimatedLpiExecutor.h"
#include "..\..\..\ValueDomainTypes.h"
#include "..\LpiExecutorParams.h"
#include "..\LpiExecutor.h"
#include <math.h>

namespace LS {
	/*!
		@brief		Provides an executor implementation for the blocks LPI.
		@author		Kevin White
		@date		9 Jan 2021
	*/
	class BlocksNonAnimatedLpiExecutor : public NonAnimatedLpiExecutor {
	public:
		virtual bool ValidateLpi(LpiExecutorParams* lpiExecParams);
		virtual void ExecuteNonAnimated(LpiExecutorParams* lpiExecParams, LpiExecutorOutput* output);
	};
}

#endif
