#include "CommandExecutor.h"

namespace LS {
	/*!
	  @brief  Factory method checks whether a command has been received by the web server.  If
			  a command has been received then a pointer to that command's handler is returned.
			  Otherwise, a nullptr is returned as there is no command.
	  @returns	 A pointer to the ICommand instance to handle the command or nullptr if no command has
				been received.
	*/
	ICommand* CommandExecutor::GetNextCommand() {
#if defined(DEBUG_COMMAND) || defined(DEBUG_ALL)
		uint32_t startCheckCommand = millis();
		appLogger->logEvent(startCheckCommand, 1, "Command", "Start Check");
#endif
		switch (lightWebServer->HandleNextCommand()) {
			case CommandType::CHECKPOWER:
#if defined(DEBUG_COMMAND) || defined(DEBUG_ALL)
				appLogger->logEvent(startCheckCommand, 1, "Command", "End Check", millis(), "Command: CheckPower");
#endif
				return checkPowerCommand;
			case CommandType::GETABOUT:
#if defined(DEBUG_COMMAND) || defined(DEBUG_ALL)
				appLogger->logEvent(startCheckCommand, 1, "Command", "End Check", millis(), "Command: GetAbout");
#endif
				return getAboutCommand;
			case CommandType::INVALID:
#if defined(DEBUG_COMMAND) || defined(DEBUG_ALL)
				appLogger->logEvent(startCheckCommand, 1, "Command", "End Check", millis(), "Command: InvalidCommand");
#endif
				return invalidCommand;
			case CommandType::LOADPROGRAM:
#if defined(DEBUG_COMMAND) || defined(DEBUG_ALL)
				appLogger->logEvent(startCheckCommand, 1, "Command", "End Check", millis(), "Command: LoadProgramCommand");
#endif
				return loadProgramCommand;
			case CommandType::NOAUTH:
#if defined(DEBUG_COMMAND) || defined(DEBUG_ALL)
				appLogger->logEvent(startCheckCommand, 1, "Command", "End Check", millis(), "Command: NoAuthCommand");
#endif
				return noAuthCommand;
			case CommandType::POWEROFF:
#if defined(DEBUG_COMMAND) || defined(DEBUG_ALL)
				appLogger->logEvent(startCheckCommand, 1, "Command", "End Check", millis(), "Command: PowerOffCommand");
#endif
				return powerOffCommand;
			case CommandType::POWERON:
#if defined(DEBUG_COMMAND) || defined(DEBUG_ALL)
				appLogger->logEvent(startCheckCommand, 1, "Command", "End Check", millis(), "Command: PowerOnCommand");
#endif
				return powerOnCommand;
			case CommandType::SETLEDS:
#if defined(DEBUG_COMMAND) || defined(DEBUG_ALL)
				appLogger->logEvent(startCheckCommand, 1, "Command", "End Check", millis(), "Command: SetLedsCommand");
#endif
				return setLedsCommand;
			default:
#if defined(DEBUG_COMMAND) || defined(DEBUG_ALL)
				appLogger->logEvent(startCheckCommand, 1, "Command", "End Check", millis(), "NO COMMAND");
#endif
				return nullptr;
		}

	}

	/*!
	  @brief   Executes the command executor.  This will cause
			   the command executor to check whether it is time
			   to execute or not.  If it is time then the current cycle
			   operators will be performed.
	  @returns bool			True if the command executor executed.  False if it is not yet time.
	*/
	bool CommandExecutor::Execute() {
		currentExecuteTime = millis();

		if (isRunning && currentExecuteTime >= nextCycleExecuteTime) {
#if defined(DEBUG_CYCLE) || defined(DEBUG_ALL)
			appLogger->logEvent(currentExecuteTime, 0, "Cycle", "Start");
#endif

			// Check if a command is waiting
			ICommand* commandHandler = GetNextCommand();

			// Execute command
			if (commandHandler != nullptr) {
#if defined(DEBUG_COMMAND) || defined(DEBUG_ALL)
				uint32_t statrExecuteCommand = millis();
				appLogger->logEvent(statrExecuteCommand, 1, "Command", "Start Execute");
#endif
				commandHandler->ExecuteCommand();
#if defined(DEBUG_COMMAND) || defined(DEBUG_ALL)
				appLogger->logEvent(statrExecuteCommand, 1, "Command", "End Execute", millis());
#endif
			}

			// Render
			riBuffer->ClearBuffer();
			bool hasInstruction = lpe->GetNextRI(riBuffer);
			if (hasInstruction) {
#if defined(DEBUG_RENDER) || defined(DEBUG_ALL)
				uint32_t startRender = millis();
				appLogger->logEvent(startRender, 1, "Render", "Start Render");
#endif
				renderer->SetRI(riBuffer->GetBuffer());
				renderer->Render();
#if defined(DEBUG_RENDER) || defined(DEBUG_ALL)
				appLogger->logEvent(startRender, 1, "Render", "End Render", millis());
#endif
			}

			nextCycleExecuteTime += CYCLE_TIME;
#if defined(DEBUG_CYCLE) || defined(DEBUG_ALL)
			appLogger->logEvent(currentExecuteTime, 0, "Cycle", "End", millis());
#endif
			return true;
		}

		return false;
	}

	/*!
	@brief   Starts the orchastrator.
	*/
	void CommandExecutor::Start() {
		isRunning = true;
	}

	/*!
	  @brief   Stops the orchastrator.
	*/
	void CommandExecutor::Stop() {
		isRunning = false;
	}
}