#include "LpiExecutorOutput.h"

namespace LS {
	/*!
		@brief		Resets the rendering instructions by setting the index
					of the rendering instruction to 0 and setting the flag
					that determines whether any rendering isntructions have
					been set to false.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	void LpiExecutorOutput::Reset() {
		renderingInstructionIndex = 0;
		renderingInstructionsSet = false;
		repeat = false;
	}

	/*!
		@brief		Adds a rendering instruction to the buffer.
		@param		colour		The colour to use to render.
		@param		numPixels	The number of pixels to be rendered.  If this is 0 then
								it means 'repeat' all rendering instructions that have
								been stored to this rendering instruction.  Repeating is useful
								when we have solid colours or simple repeating patterns.
		@author		Kevin White
		@date		2 Jan 2021
	*/
	void LpiExecutorOutput::SetNextRenderingInstruction(Colour* colour, uint8_t numPixels) {
		renderingInstructions[renderingInstructionIndex].colour.SetFromColour(colour);
		renderingInstructions[renderingInstructionIndex++].number = numPixels;
		renderingInstructionsSet = true;
	}

	/*!
		@brief		Gets whether at least one rendering instruction has been set.
		@returns	True if at least one RI has been set or false if non have been set.
		@author		Kevin White
		@date		3 Jan 2021
	*/
	bool LpiExecutorOutput::RenderingInstructionsSet() {
		return renderingInstructionsSet;
	}

	/*!
		@brief		Gets a pointer to the collection of rendering instructions.
		@returns	A pointer to the collection of rendering instructions.
		@author		Kevin White
		@date		3 Jan 2021
	*/
	RI* LpiExecutorOutput::GetRenderingInstructions() {
		return renderingInstructions;
	}

	/*!
		@brief		Gets the number of rendering instructions that have been set.
		@rturns		The number of rendering instructions that have been set.
		@author		Kevin White
		@date		3 Jan 2021
	*/
	uint8_t LpiExecutorOutput::GetNumberOfRenderingInstructions() {
		return renderingInstructionIndex;
	}

	/*!
		@brief		Sets a flag that specifies that the rendering instructions
					should be repeated when the last instruction has
					been read.  This is useful for values that repeat themselve
					along the line of LEDs i.e. solid colour or simple patterns.
		@author		Kevin White
		@date		4 Jan 2021
	*/
	void LpiExecutorOutput::SetRepeatRenderingInstructions() {
		repeat = true;
	}

	/*!
		@brief		Gets a value that determines wether the rendering
					instructions should be re-read once the end has been
					reached.  Useful for things like solid colours
					or simple patterns that only require 1 or a few
					rendering instructions and can then simply
					be repeated.
		@author		Kevin White
		@date		4 Jan 2021
	*/
	bool LpiExecutorOutput::GetRepeatRenderingInstructions() {
		return repeat;
	}
}