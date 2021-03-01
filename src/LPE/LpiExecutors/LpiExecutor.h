#ifndef _LpiExecutor_h
#define _LpiExecutor_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include "LpiExecutorOutput.h"
#include "LpiExecutorParams.h"

namespace LS {
	/*!
		@brief		Abstract base-class for a class that executes an LPI.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	class LpiExecutor {
	public:
		virtual bool ValidateLpi(LpiExecutorParams* lpiExecParams) = 0;
		virtual uint16_t GetNumberOfSteps(LpiExecutorParams* lpiExecParams) = 0;
		virtual void Execute(LpiExecutorParams* lpiExecParams, uint16_t step, LpiExecutorOutput* output) = 0;
	};
}

#endif
