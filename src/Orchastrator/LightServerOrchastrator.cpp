#include "LightServerOrchastrator.h"

namespace LS {
	/*!
		@brief	Constructor injects dependencies.
		@param	timer			A reference to the class that provides timing functions for the orchastrator.
		@param	lpExecutor		A reference to the class that executes light programs.
		@param	primaryLpState	A reference to the class that contains the tree state representation of the primary light program.
		@param	pixelRenderer	A reference to the class that interacts with the connected LED hardware.
		@param	webServer		A reference to the class that receives and sends HTTP messages.
		@param	commmandFactory	A reference to the class that gets references to individual commands that are executed in response to HTTP messages received.
		@author	Kevin White
		@date	8 Feb 21
	*/
	LightServerOrchastrator::LightServerOrchastrator(
		Timer* timer, 
		LpExecutor* lpExecutor, 
		LpState* primaryLpState, 
		PixelRenderer* renderer, 
		ILightWebServer* webServer, 
		CommandFactory* commandFactory
	) {
		this->timer = timer;
		this->lpExecutor = lpExecutor;
		this->primaryLpState = primaryLpState;
		this->renderer = renderer;
		this->webServer = webServer;
		this->commandFactory = commandFactory;
	}

	/*!
		@brief	Stops any programs from executing by resetting the execution state
				of those programs.
		@date	21 Jan 21
		@author	Kevin White
	*/
	void LightServerOrchastrator::StopPrograms() {
		// resetting an LP state effectively 'stops' the program as there are no further
		// instructions to be executed as the state is cleared.
		primaryLpState->reset();
	}

	/*!
		@brief	Stops the orchastrator from further execution cycles.
		@date	5 Feb 21
		@author	Kevin White
	*/
	void LightServerOrchastrator::Stop() {
		isRunning = false;
	}

	/*!
		@brief	Starts the orchastrator so that commands are executed
				on future execution cycles.
		@date	5 Feb 21
		@author	Kevin White
	*/
	void LightServerOrchastrator::Start() {
		isRunning = true;
	}

	// NOTE: There are two versions of the Execute method:
	// (1) for when no debugging output is required.  This is a 'clean' method without any debugging output statements.
	// (2) for when debugging output is required.  This contains additional code to cause debug messages to be sent via the serial connection.
	//     The additional statements are contained in /** START: DEBUG **/ and /** END: DEBUG **/ blocks.
	// If any changes are made to (1) then they should be reflected in (2).
#ifndef ORCHASTRATOR_DEBUG_ALL
	/*!
			@brief	Executes the orchastrator which causes the following actions:
					1.
			@returns	True if the orchastrator was executed.  False if the orchastrator is not running or it is not time to execute.
			@author	Kevin White
			@date	5 Feb 21
		*/
	bool LightServerOrchastrator::Execute(bool isInSetupMode) {
		bool timeToExecute = timer->IsTime();

		if (!timeToExecute || !isRunning) {
			return false;
		}

		// see if there's a RI to be rendered
		lpExecutor->Execute(primaryLpState, &lpiExecutorOutput);
		if (lpiExecutorOutput.RenderingInstructionsSet()) {
			// there's a RI to be rendered...so render it
			renderer->SetPixels(&lpiExecutorOutput);
			renderer->ShowPixels();
		}

		if (isInSetupMode) {
			// do not execute the web server if in set up mode because
			// the LDL web server can conflict with the set up web portal
			// and cause it to be become non-responsive, requiring multiple restarts
			return false;
		}
		
		// see if a new command has been received
		CommandType nextCommand = webServer->HandleNextCommand();

		// new command received...so execute it (could be a new LP, for example)
		if (nextCommand != CommandType::NONE) {
			ICommand* nextCommandToExecute = commandFactory->GetCommand(nextCommand);

			if (nextCommandToExecute != nullptr) {
				if (!nextCommandToExecute->ExecuteCommand()) {
					// TODO: what do we do here?
				}
			}
		}

		return true;
	}
#endif

#ifdef ORCHASTRATOR_DEBUG_ALL
	/*!
		@brief	Executes the orchastrator which causes the following actions:
				1.
		@returns	True if the orchastrator was executed.  False if the orchastrator is not running or it is not time to execute.
		@author	Kevin White
		@date	5 Feb 21
	*/
	bool LightServerOrchastrator::Execute(bool isInSetupMode) {
		bool timeToExecute = timer->IsTime();

		if (!timeToExecute || !isRunning) {
			return false;
		}

		/** START: DEBUG **/
		uint32_t startExecuteCycle = millis();
		appLogger->logEvent(startExecuteCycle, 1, "Cycle", "Execute", true, startExecuteCycle);
		uint32_t startRendering = millis();
		appLogger->logEvent(startRendering, 2, "Render", "Render", true, startRendering);
		/** END: DEBUG **/

		// see if there's a RI to be rendered
		lpExecutor->Execute(primaryLpState, &lpiExecutorOutput);
		if (lpiExecutorOutput.RenderingInstructionsSet()) {
			// there's a RI to be rendered...so render it
			renderer->SetPixels(&lpiExecutorOutput);
			renderer->ShowPixels();
			/** START: DEBUG **/
			uint32_t startRendering = millis();
			appLogger->logEvent(startExecuteCycle, 2, "Render", "Execute", false, millis());
			/** END: DEBUG **/
		}
		/** START: DEBUG **/
		else {
			uint32_t startRendering = millis();
			appLogger->logEvent(startExecuteCycle, 2, "Render", "Execute", false, millis(), "NOTHING TO RENDER");
		}
		/** END: DEBUG **/

		if (isInSetupMode) {
			// do not execute the web server if in set up mode because
			// the LDL web server can conflict with the set up web portal
			// and cause it to be become non-responsive, requiring multiple restarts
			return false;
		}


		// see if a new command has been received
		CommandType nextCommand = webServer->HandleNextCommand();

		// new command received...so execute it (could be a new LP, for example)
		if (nextCommand != CommandType::NONE) {
			ICommand* nextCommandToExecute = commandFactory->GetCommand(nextCommand);
			/** START: DEBUG **/
			uint32_t startExecuteCommand = millis();
			if (nextCommandToExecute != nullptr) {
				appLogger->logEvent(startExecuteCommand, 2, "Command", "Execute", true, startExecuteCommand);
			}
			/** END: DEBUG **/

			if (nextCommandToExecute != nullptr) {
				if (!nextCommandToExecute->ExecuteCommand()) {
					// TODO: what do we do here?
					/** START: DEBUG **/
					appLogger->logEvent(startExecuteCommand, 2, "Command", "Execute", false, millis(), "COMMAND FAILED TO EXECUTE");
					/** END: DEBUG **/
				}
				/** START: DEBUG **/
				appLogger->logEvent(startExecuteCommand, 2, "Command", "Execute", false, millis());
				/** END: DEBUG **/
			}
		}

		/** START: DEBUG **/
		appLogger->logEvent(startExecuteCycle, 1, "Cycle", "Execute", false, millis());
		/** END: DEBUG **/

		return true;
	}
#endif
}