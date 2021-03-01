#include "LpiExecutorFactory.h"

namespace LS {
	/*!
		@brief	Adds the available lpi executors to the collection of executors.
		@author	Kevin Whtite
		@date	13 Jan 2021
	*/
	LpiExecutorFactory::LpiExecutorFactory() {
		lpiExecutors[LpiOpCode::Clear] = new ClearNonAnimatedLpiExecutor();
		lpiExecutors[LpiOpCode::Solid] = new SolidNonAnimatedLpiExecutor();
		lpiExecutors[LpiOpCode::Pattern] = new PatternNonAnimatedLpiExecutor();
		lpiExecutors[LpiOpCode::Slider] = new SliderAnimatedLpiExecutor();
		lpiExecutors[LpiOpCode::Fade] = new FadeAnimatedLpiExecutor();
		lpiExecutors[LpiOpCode::Stochastic] = new StochasticNonAnimatedLpiExecutor();
		lpiExecutors[LpiOpCode::Blocks] = new BlocksNonAnimatedLpiExecutor();
		lpiExecutors[LpiOpCode::Rainbow] = new RainbowAnimatedLpiExecutor();
	}

	/*!
		@brief	Destructor frees up allocated memory.
		@author	Kevin Whtite
		@date	13 Jan 2021
	*/
	LpiExecutorFactory::~LpiExecutorFactory() {
		free(lpiExecutors[LpiOpCode::Clear]);
		free(lpiExecutors[LpiOpCode::Solid]);
		free(lpiExecutors[LpiOpCode::Pattern]);
		free(lpiExecutors[LpiOpCode::Slider]);
		free(lpiExecutors[LpiOpCode::Fade]);
		free(lpiExecutors[LpiOpCode::Stochastic]);
		free(lpiExecutors[LpiOpCode::Blocks]);
		free(lpiExecutors[LpiOpCode::Rainbow]);
	}

	/*!
		@brief	Gets the LPI executor instance for the given LPI op-code.
		@param	opCode	The LPI op-code of the LPI executor to be returned
		@returns		A pointer to the LPI executor instance if a valid op-code
						was supplied or nullptr if an invalid op-code as supplied.
		@author			Kevin White
		@date			13 Jan 2021
	*/
	LpiExecutor* LpiExecutorFactory::GetLpiExecutor(uint8_t opCode) {
		switch(opCode) {
			case LpiOpCode::Clear:
			case LpiOpCode::Solid:
			case LpiOpCode::Pattern:
			case LpiOpCode::Slider:
			case LpiOpCode::Fade:
			case LpiOpCode::Stochastic:
			case LpiOpCode::Blocks:
			case LpiOpCode::Rainbow:
				return lpiExecutors[opCode];
		}

		return nullptr;
	}
}