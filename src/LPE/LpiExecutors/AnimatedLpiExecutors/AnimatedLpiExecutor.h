#ifndef _AnimatedLpiExecutor_h
#define _AnimatedLpiExecutor_h

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
					animated.  For example: the slider animation has several steps.
		@author		Kevin White
		@date		4 Jan 2021
	*/
	class AnimatedLpiExecutor : public LpiExecutor {
	};
}

#endif
