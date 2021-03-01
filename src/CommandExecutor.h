/*!
 * @file CommandExecutor.h
 *
 * Orchastrating instance that controls
 * the execution of the server and implements
 * the rendering execution cycle.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _COMMANDEXECUTOR_H
#define _COMMANDEXECUTOR_H

#include "StringProcessor.h"
#include "ValueDomainTypes.h"
#include "Renderer.h"
#include "LPE.h"
#include "Commands/InvalidCommand.h"
#include "Commands/CheckPowerCommand.h"
#include "Commands/GetAboutCommand.h"
#include "Commands/LoadProgramCommand.h"
#include "Commands/NoAuthCommand.h"
#include "Commands/PowerOffCommand.h"
#include "Commands/PowerOnCommand.h"
#include "Commands/SetLedsCommand.h"
#include "AppLogger.h"

namespace LS {

	#ifdef __arm__
		#define BUFFER_SIZE		10000	// Due
	#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
		#define BUFFER_SIZE		1000	// Mega
	#endif  // __arm__

	// #define DEBUG_ALL					// define to output ALL debugging messages (this will add overhead to the processing!)

	//#define DEBUG_CYCLE			// define to output cycle debugging message
	//#define DEBUG_COMMAND			// define to output command debugging message
	//#define DEBUG_RENDER			// define to output render debugging message

	#define CYCLE_TIME			50	// period of each execution cycle in milliseconds.  the normal value is 50 but other values may be used for debugging.

	/*!
	@brief  Orchastrating instance for the Light Sever: ensures
			correct timing of events, executes the rendering cycle,
			handles API requests and responses, etc.
	*/
	class CommandExecutor
	{
	private:
		bool isRunning = false;

		uint32_t nextCycleExecuteTime = 0;
		uint32_t currentExecuteTime = 0;

		StringProcessor stringProcessor = StringProcessor();

		IPixelController* pixelController = nullptr;
		LEDConfig* ledConfig = nullptr;

		Renderer* renderer = nullptr;
		LPE* lpe = nullptr;
		IAppLogger* appLogger = nullptr;

		// Buffers used to execute LP programs
		FixedSizeCharBuffer* riBuffer = nullptr;				// Render Instruction (RI) buffer: contains the translation of LPIs into RIs
		FixedSizeCharBuffer* lpBuffer = nullptr;				// Light Program (LP) buffer: stores a Light Program so that it can be executed
		FixedSizeCharBuffer* webBuffer = nullptr;				// Web request buffer: stores the incoming web request
		FixedSizeCharBuffer* webResponseBuffer = nullptr;		// Web reponse bufer: stores the outgoing web request
		StaticJsonDocument<1000> webResponseDoc;				// Web response document: used to consturct a JSON object to be sent in the body of a web reponse

		ILightWebServer* lightWebServer = nullptr;				// Reference to the class that processes web requests (effectively, this is the API)

		/* The following are the references to the classes that handle API requests */
		InvalidCommand* invalidCommand = nullptr;
		NoAuthCommand* noAuthCommand = nullptr;
		PowerOffCommand* powerOffCommand = nullptr;
		PowerOnCommand* powerOnCommand = nullptr;
		LoadProgramCommand* loadProgramCommand = nullptr;
		CheckPowerCommand* checkPowerCommand = nullptr;
		GetAboutCommand* getAboutCommand = nullptr;
		SetLedsCommand* setLedsCommand = nullptr;

		/*!
		  @brief  Factory method checks whether a command has been received by the web server.  If
				  a command has been received then a pointer to that command's handler is returned.
				  Otherwise, a nullptr is returned as there is no command.
		  @returns	 A pointer to the ICommand instance to handle the command or nullptr if no command has
					been received.
		*/
		ICommand* GetNextCommand();
	public:
		/*!
		  @brief   Constructor injects the mandatory dependencioes.
		  @param   pixelController		Pointer to the class that controls the connected LEDs.
		  @param   ledConfig			Pointer to the class that encapsulates the configuration of the connected LEDs.
		*/
		CommandExecutor(
			IPixelController* pixelController,
			LEDConfig* ledConfig,
			ILightWebServer* lightWebServer,
			IAppLogger* appLogger
		) {
			this->pixelController = pixelController;
			this->ledConfig = ledConfig;
			this->lightWebServer = lightWebServer;
			this->appLogger = appLogger;

			renderer = new Renderer(&stringProcessor, ledConfig, pixelController);

			riBuffer = new FixedSizeCharBuffer(BUFFER_SIZE);
			lpBuffer = new FixedSizeCharBuffer(BUFFER_SIZE);
			webBuffer = new FixedSizeCharBuffer(BUFFER_SIZE);
			webResponseBuffer = new FixedSizeCharBuffer(1000);

			lpe = new LPE(ledConfig, &stringProcessor);

			/* Create instances of each of the commands */
			invalidCommand = new InvalidCommand(lightWebServer);
			noAuthCommand = new NoAuthCommand(lightWebServer);
			// powerOffCommand = new PowerOffCommand(lightWebServer, pixelController, lpe);
			//powerOnCommand = new PowerOnCommand(lightWebServer, pixelController, lpe, &stringProcessor);
			// loadProgramCommand = new LoadProgramCommand(lightWebServer, lpe, lpBuffer);
			checkPowerCommand = new CheckPowerCommand(lightWebServer, pixelController, &webResponseDoc, webResponseBuffer);
			// getAboutCommand = new GetAboutCommand(lightWebServer, &webResponseDoc, webResponseBuffer);
			setLedsCommand = new SetLedsCommand(lightWebServer, &stringProcessor);
		}

		/*!
		  @brief   Destructor cleans-up allocated resources.
		*/
		~CommandExecutor() {
			delete riBuffer;
			delete lpBuffer;
			delete webBuffer;
			delete webResponseBuffer;

			delete renderer;

			delete invalidCommand;
			delete noAuthCommand;
			delete powerOffCommand;
			delete powerOnCommand;
			delete loadProgramCommand;
			delete checkPowerCommand;
			delete getAboutCommand;
			delete setLedsCommand;
		}

		/*!
		  @brief   Executes the command executor.  This will cause
				   the command executor to check whether it is time
				   to execute or not.  If it is time then the current cycle
				   operators will be performed.
		  @returns bool			True if the command executor executed.  False if it is not yet time.
		*/
		bool Execute();

		/*!
		  @brief   Starts or restarts the orchastrator.
		*/
		void Start();

		/*!
		  @brief   Stops the orchastrator.
		*/
		void Stop();
	};
}
#endif