#ifndef _SolidNonAnimatedLpiExecutor_h
#define _SolidNonAnimatedLpiExecutor_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\..\WProgram.h"
#endif

#include "NonAnimatedLpiExecutor.h"

namespace LS {
	/*!
		@brief		Provides an executor implementation for the solid LPI.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	class SolidNonAnimatedLpiExecutor : public NonAnimatedLpiExecutor {
	public:
		virtual bool ValidateLpi(LpiExecutorParams* lpiExecParams);
		virtual void ExecuteNonAnimated(LpiExecutorParams* lpiExecParams, LpiExecutorOutput* output);
	};
}

#endif
