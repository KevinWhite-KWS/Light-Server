#ifndef _NonAnimatedLpiExecutor_h
#define _NonAnimatedLpiExecutor_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\..\WProgram.h"
#endif

#include "..\LpiExecutor.h"
#include "..\LpiExecutorParams.h"

namespace LS {
	/*!
		@brief		Abstract base-class for an LPI executor that is
					non-animated.  For example: the clear animation has a single step.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	class NonAnimatedLpiExecutor : public LpiExecutor {
	protected:
		virtual void ExecuteNonAnimated(LpiExecutorParams* lpiExecParams, LpiExecutorOutput* output) = 0;
	public:
		virtual uint16_t GetNumberOfSteps(LpiExecutorParams* lpiExecParams);

		virtual void Execute(LpiExecutorParams* lpiExecParams, uint16_t step, LpiExecutorOutput* output);
	};
}

#endif
