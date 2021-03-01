#ifndef _LpiExecutorFactory_h
#define _LpiExecutorFactory_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include "LpiExecutor.h"
#include "AnimatedLpiExecutors/FadeAnimatedLpiExecutor.h"
#include "AnimatedLpiExecutors/RainbowAnimatedLpiExecutor.h"
#include "AnimatedLpiExecutors/SliderAnimatedLpiExecutor.h"
#include "NonAnimatedLpiExecutors/BlocksNonAnimatedLpiExecutor.h"
#include "NonAnimatedLpiExecutors/ClearNonAnimatedLpiExecutor.h"
#include "NonAnimatedLpiExecutors/PatternNonAnimatedLpiExecutor.h"
#include "NonAnimatedLpiExecutors/SolidNonAnimatedLpiExecutor.h"
#include "NonAnimatedLpiExecutors/StochasticNonAnimatedLpiExecutor.h"
#include "..\InstructionType.h"

namespace LS {
	enum LpiOpCode{
		Clear,
		Solid,
		Pattern,
		Slider,
		Fade,
		Stochastic,
		Blocks,
		Rainbow
	};

	/*!
		@brief	Factory class gets individual LPI executor instances
				using the op-code of the LPI.
		@author	Kevin White
		@date	13 Jan 2021
	*/
	class LpiExecutorFactory {
	private:
		LpiExecutor* lpiExecutors[8];

	public:
		LpiExecutorFactory();
		~LpiExecutorFactory();

		LpiExecutor* GetLpiExecutor(uint8_t opCode);
	};
}


#endif