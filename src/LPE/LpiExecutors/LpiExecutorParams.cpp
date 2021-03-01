#include "LpiExecutorParams.h"

namespace LS {
	/*!
		@brief		Resets this instance with new values to be used to pass
					to classes that execute LPIs.
		@param		lpiBuffer		A pointer to the string buffer that contains the LPI to be executed.
		@param		ledConfig		A pointer to the instance that specifies details about the LED configuration.
		@param		stringProcessor	A pointer to the instance that provides string parsing functionality.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	void LpiExecutorParams::Reset(FixedSizeCharBuffer* lpiBuffer, LEDConfig* ledConfig, StringProcessor* stringProcessor) {
		this->lpiBuffer = lpiBuffer;
		this->ledConfig = ledConfig;
		this->stringProcessor = stringProcessor;
	}

	/*!
		@brief		Gets a pointer to the string buffer that contains the LPI to be executed.
		@returns	A pointer to the LPI string to be executed.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	FixedSizeCharBuffer* LpiExecutorParams::GetLpiBuffer() {
		return lpiBuffer;
	}

	/*!
		@brief		Gets a pointer to the portion of the LPI buffer with the basic
					details (instruction #, duration, reserved bytes) skipped over.  In another,
					at the details of the specific LPI to be executed.
		@returns	A pointer to the specific LPI instruction details.
		@author		Kevin White
		@date		2 Jan 2020
	*/
	const char* LpiExecutorParams::GetLpiBufferWithoutBasicDetails() {
		return lpiBuffer->GetBuffer() + BASIC_LPI_DETAILS_LENGTH;
	}

	/*!
		@brief		Gets a pointer to the instance that specifies details about the LED configuration.
		@returns	A pointer to the instance that contains details about the LED configuration.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	LEDConfig* LpiExecutorParams::GetLedConfig() {
		return ledConfig;
	}

	/*!
		@brief		Gets a pointer to instance that provides string parsing functionality.
		@returns	A pointer to the class that provides string parsing functionality.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	StringProcessor* LpiExecutorParams::GetStringProcesor() {
		return stringProcessor;
	}
}