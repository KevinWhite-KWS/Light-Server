#ifndef _LpiExecutorOutput_h
#define _LpiExecutorOutput_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include "..\..\ValueDomainTypes.h"

namespace LS {
	/*!
		@brief		Stores the output from executing an LPI.  The output consists
					of one or more rendering instructions which are simply colours
					and number of pixels to be rendered.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	class LpiExecutorOutput {
		RI renderingInstructions[200];
		uint8_t renderingInstructionIndex = 0;
		bool renderingInstructionsSet = false;
		bool repeat = false;
	public:
		void Reset();
		void SetNextRenderingInstruction(Colour* colour, uint8_t numPixels);
		bool RenderingInstructionsSet();
		RI* GetRenderingInstructions();
		uint8_t GetNumberOfRenderingInstructions();

		void SetRepeatRenderingInstructions();
		bool GetRepeatRenderingInstructions();
	};
}

#endif
