#ifndef _LpExecutor_h
#define _LpExecutor_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include "..\StateBuilder\LpState.h"
#include "..\..\FixedSizeCharBuffer.h"
#include "..\..\ValueDomainTypes.h"
#include "..\LpiExecutors\LpiExecutorFactory.h"
#include "..\LpiExecutors\LpiExecutorOutput.h"

namespace LS {
	/**
	* @brief	Factory class containing a factory method to get
	*			a particular instance of a LPI.
	* @author	Kevin White
	* @date		14 Dec 2020
	*/
	class LpExecutor {
	private:
		LpiExecutorFactory* lpiFactory = nullptr;
		StringProcessor* stringProcessor;
		LEDConfig* ledConfig;
		LpiExecutorParams lpiExecutorParams;
		
		// 500:  *** BUFFER ALLOCATION *** - Individual LPI loading buffer
		FixedSizeCharBuffer lpiBuffer = FixedSizeCharBuffer(BUFFER_LPI_LOADING);
		LPIInstruction basicLpiDetails;
	protected:
		bool RenderCurrentInstruction(Instruction* currentInstruction, LpiExecutorOutput* lpiExecutorOutput);
		void NavigateToNextInstruction(LpState* state);
		void NavigateDownToFirstLp(LpState* state);

	public:
		LpExecutor(LpiExecutorFactory* lpiExecutorFactory, StringProcessor* stringProcessor, LEDConfig* ledConfig);

		virtual void Execute(LpState* state, LpiExecutorOutput* lpiExecutorOutput);
	};
}
#endif