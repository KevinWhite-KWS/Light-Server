#ifndef _LpiExecutorParams_h
#define _LpiExecutorParams_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include "..\..\ValueDomainTypes.h"
#include "..\..\StringProcessor.h"
#include "..\..\FixedSizeCharBuffer.h"

#define BASIC_LPI_DETAILS_LENGTH	8

namespace LS {
	/*!
		@brief		Simple class for passing the basic parameters
					necessary to execute an LPI.
		@author		Kevin White
		@date		2 Jan 2020
	*/
	class LpiExecutorParams {
	private:
		FixedSizeCharBuffer* lpiBuffer;
		LEDConfig* ledConfig;
		StringProcessor* stringProcessor;
	public:
		void Reset(FixedSizeCharBuffer* lpiBuffer, LEDConfig* ledConfig, StringProcessor* stringProcessor);
		FixedSizeCharBuffer* GetLpiBuffer();
		const char* GetLpiBufferWithoutBasicDetails();
		LEDConfig* GetLedConfig();
		StringProcessor* GetStringProcesor();
	};
}

#endif
