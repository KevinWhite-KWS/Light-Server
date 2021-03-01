#include "LpExecutor.h"

namespace LS {
	/*!
		@brief	Default constructor injects dependencies
		@param	lpiFactory		A pointer to the factory class that provides access to individual LPI instruction instances.
		@param	stringProcessor	A pointer to the class that provides string parsing.
		@param	ledConfig		A pointer to the class that contains configuration data related to the LEDs.
		@author	Kevin White
		@date	29 Dec 2020
	*/
	LpExecutor::LpExecutor(LpiExecutorFactory* lpiFactory, StringProcessor* stringProcessor, LEDConfig* ledConfig) {
		this->lpiFactory = lpiFactory;
		this->stringProcessor = stringProcessor;
		this->ledConfig = ledConfig;
		lpiExecutorParams.Reset(&lpiBuffer, this->ledConfig, this->stringProcessor);
	}

	/*!
		
	*/
	bool LpExecutor::RenderCurrentInstruction(Instruction* currentInstruction, LpiExecutorOutput* lpiExecutorOutput) {
		LpInstruction* lpInstruction = (LpInstruction*)currentInstruction;

		// Lets first see if the this instruction is already complete i.e
		// rendered with no further frames to be rendered (for animated instructions)
		if (lpInstruction->GetDuration() == 0 
			&& !lpInstruction->HasMoreSteps()) {
			// this instruction complete, so we need to navigate to the next
			return true;
		}

		// Now, is it time to render this instrution.  We determine this if the
		// following is true:
		//		1. the currentDuration is the value of the duration specified in the LPI, AND
		//		2. the number of remainingSteps is not 0 (i.e. last frame NOT already rendered).
		// The duration of an LPI determines for how long an effect remains active in
		// terms of rendering frames.  Thus, we render when currentDuration is the value
		// of duration and then, on each subsequent call, do nothing more until the
		// duration is reset.  This is because the LEDs will have been rendered and we
		// do not need to change them until the duration of the effect is complete.
		if (lpInstruction->IsTimeToRender() 
			&& lpInstruction->HasMoreSteps()) {
			// get the basic LPI details incluing op-code
			stringProcessor->ExtractLPIFromHexEncoded(lpInstruction->getLpi(), &basicLpiDetails);

			// render the LPI
			lpiBuffer.LoadFromBuffer(lpInstruction->getLpi());
			// LPI* lpi = lpiFactory->GetLPI(&lpiBuffer, &basicLpiDetails);
			LpiExecutor* lpiExecutor = lpiFactory->GetLpiExecutor(basicLpiDetails.opcode);

			// TODO: fix the need to validate the instruction each time
			// lpi->Reset(&basicLpiDetails);
			// bool rendered = lpi->GetNextRI(renderingBuffer);
			lpiExecutor->Execute(&lpiExecutorParams, lpInstruction->GetCurrentStep(), lpiExecutorOutput);
		}

		// reduce the currentDuration of the current instruction by 1
		// i.e. this execution is one duration cycle
		uint8_t currentDuration = lpInstruction->DecrementCurrentDuration();

		if (currentDuration == 0 
			&& lpInstruction->HasMoreSteps()) {
			// reset the duration and move to the next animation step
			lpInstruction->ResetDuration();
			lpInstruction->DecrementSteps();
		}

		// we only move to the next LPI is the current LPI is fully complete i.e.
		// all steps in an animation effect have been rendered AND duration has counted down
		bool moveToNextInstruction = !lpInstruction->HasMoreSteps() && currentDuration == 0;

		return moveToNextInstruction;
	}

	/*!
		@brief		Moves 'down' the tree until the next LPI is encountered.  This is
					used when instructions are executing that have children instructions
					i.e. repeats.  We move to the first child of a repeat and then
					check whether that is another repeat.  If so then we must continue
					moving down and so on until we get to the first child of a repeat
					which is an LPI.  The first LPI that is encountered is set as the
					currentInstruction in the LP state.
		@param		state	The LP state
		@author		Kevin White
		@date		31 Dec 2020
	*/
	void LpExecutor::NavigateDownToFirstLp(LpState* state) {
		Instruction* currentInstruction = state->getCurrentInstruction();

		while (currentInstruction->getInstructionType() != InstructionType::Lpi) {
			// currentInstruction must have an instruction that has child instructions
			// so we need to get the first child to check whether it is an LPI
			currentInstruction = ((InstructionWithChild*)currentInstruction)->getFirstChild();

			if (currentInstruction->getInstructionType() == InstructionType::Repeat) {
				// however, if its a repeat then we need to reset the
				// loop count as it is a new execution over that loop
				((RepeatInstruction*)currentInstruction)->ResetRemainingIterations();
			}
		}

		// reset duration of LPI each time LPI is executed
		((LpInstruction*)currentInstruction)->ResetDurationAndSteps();
		state->setCurrentInstruction(currentInstruction);
	}

	/*!
		@brief		Moves to the next LPI instruction in an LP.  The algorithm for this
					is as follows:
						1. if currentInstruction has a sibling, move to the sibling.
							1.a If this sibling is a repeat then navigate 'down' until
								we encounter the first LPI.
						2. otherwise, get the parent of the currentInstruction (must be a Repeat):
							2.a If no parent, do nothing and return.
							2.b If remaining iterations (or is infinite loop) on repeat then navigate 'down' to first LPI.
							2.c Otherwise, recursively call this method to either move to the next
							    instruction or the parent.
	*/
	void LpExecutor::NavigateToNextInstruction(LpState* state) {
		Instruction* currentInstruction = state->getCurrentInstruction();
		state->setCurrentInstruction(nullptr);	// set to nullptr or otherwise previous instruction could be executed again

		// 1. if currentInstruction has a sibling, move to the sibling.
		if (currentInstruction->getNext() != nullptr) {
			currentInstruction = currentInstruction->getNext();
			if (currentInstruction->getInstructionType() == InstructionType::Repeat) {
				// reset loop iterations as new execution of repeat
				((RepeatInstruction*)currentInstruction)->ResetRemainingIterations();
			}
			
			// 1.a If this sibling is a repeat then navigate 'down' until we encounter the first LPI.
			state->setCurrentInstruction(currentInstruction);
			NavigateDownToFirstLp(state);
			return;
		}
		
		currentInstruction = currentInstruction->getParent();
		if(currentInstruction != nullptr 
			&& currentInstruction->getInstructionType() == InstructionType::Repeat) {
			state->setCurrentInstruction(currentInstruction);

			// 2. otherwise, get the parent of the currentInstruction (must be a Repeat):
			RepeatInstruction* repeatIns = (RepeatInstruction*)currentInstruction;

			if (repeatIns->isInfinite()
				|| repeatIns->decrementRemainingIterations() > 0) {
				// 2.b If remaining iterations(or is infinite loop) on repeat then navigate 'down' to first LPI.
				NavigateDownToFirstLp(state);
			}
			else {
				// 2.c Otherwise, recursively call this method to either move to the next
				// instruction or the parent.
				NavigateToNextInstruction(state);
			}
		}
	}

	/*!
		@brief		Executes the LP by causing the current LPI to be rendered (if we have
					an LPI then the renderingBuffer will be filled with the rendered output)
					and points the currentInstruction pointer to the next LPI to be executed
					on the next call to this method.
					This method manipulates the passed state object instance and sets the
					currentInstruction pointer in the state to the next instruction to be executed
					after the currentInstruction has been rendered.
		@param		state			Pointer to the object that stores the state of the LP as it executes
		@param		renderingBuffer	Pointer to the buffer that stores the rendered instruction output
		@author		Kevin White
		@date		31 Dec 2020
	*/
	void LpExecutor::Execute(LpState* state, LpiExecutorOutput* lpiExecutorOutput) {
		if (state == nullptr 
			|| lpiExecutorOutput == nullptr) {
			return;
		}

		// Reset the rendering buffer so that we do not return the previous
		// buffer content if a new one is not rendered
		lpiExecutorOutput->Reset();

		// render the current instruction (if any as the state may have reached the end of program)
		Instruction* currentInstruction = state->getCurrentInstruction();
		if (currentInstruction == nullptr) {
			// no current instruction so nothing to do - cannot render or navigate.  Probably
			// the previous program has come to an end.
			return;
		}

		if (currentInstruction->getInstructionType() == InstructionType::Repeat) {
			// first instruction in program may be a repeat so we need to navigate
			// to the first actual LP.  This should only ever occur once when a
			// LP is being executed.
			NavigateDownToFirstLp(state);
			currentInstruction = state->getCurrentInstruction();
		}

		bool navigateToNextInstructon = true;
		if (currentInstruction->getInstructionType() == InstructionType::Lpi) {
			// execute the current instruction (i.e. render it it time to render).  The variable
			// navigateToNextInstruction will be returned as true if the Lpi has finished rendering
			// i.e. all animation steps complete (for animated LPIs) and duration has been
			// reduced to 0.
			navigateToNextInstructon = RenderCurrentInstruction(currentInstruction, lpiExecutorOutput);
		}

		if (navigateToNextInstructon) {
			// navigate to the next LPI instruction, ready for the next cycle.  If there are no
			// further instructions then the currentInstruction pointer will be state to nullptr.
			NavigateToNextInstruction(state);
		}
	}
}