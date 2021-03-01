#ifndef _ClearNonAnimatedLpiExecutor_h
#define _ClearNonAnimatedLpiExecutor_h

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
		@brief		Provides an executor implementation for the clear LPI.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	class ClearNonAnimatedLpiExecutor : public NonAnimatedLpiExecutor {
	public:
		virtual bool ValidateLpi(LpiExecutorParams* lpiExecParams);
		virtual void ExecuteNonAnimated(LpiExecutorParams* lpiExecParams, LpiExecutorOutput* output);
	};
}

#endif
