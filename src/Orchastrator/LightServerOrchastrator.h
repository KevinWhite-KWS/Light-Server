#ifndef _LightServerOrchastor_H
#define _LightServerOrchastor_H

#include "IOrchastor.h"

#include "Timer.h"
#include "../LPE/Executor/LpExecutor.h"
#include "../LPE/StateBuilder/LpState.h"
#include "../LPE/LpiExecutors/LpiExecutorOutput.h"
#include "../Renderer/PixelRenderer.h"
#include "../DomainInterfaces.h"
#include "../Commands/CommandFactory.h"

namespace LS {
	#define		ORCHASTRATOR_DEBUG_ALL		// define this to see orchastrator debugging output
	// #define		ORCHASTRATOR_DEBUG_CYCLE	// define this to see orchastrator cycle-related debugging output
	// #define		ORCHASTRATOR_DEBUG_COMMANDS	// define this to see command execution debugging output
	// #define		ORCHASTRATOR_DEBUG_RENDERING	// define this to see orchastrator rendering-related debugging output

	class LightServerOrchastrator : public IOrchastor {
		private:
			Timer* timer;
			LpExecutor* lpExecutor;
			LpState* primaryLpState;
			PixelRenderer* renderer;
			ILightWebServer* webServer;
			CommandFactory* commandFactory;
			IAppLogger* appLogger;

		protected:
			LpiExecutorOutput lpiExecutorOutput;
			bool isRunning = true;

		public:
			LightServerOrchastrator(
				Timer* timer, 
				LpExecutor* lpExecutor, 
				LpState* primaryLpState, 
				PixelRenderer* renderer, 
				ILightWebServer* webServer, 
				CommandFactory* commandFactory
			);

			/*!
				@brief		Sets the reference to the class to be used for logging.
				@param		appLogger	The class that will perform logging.
				@author		Kevin White
				@date		15 Feb 2021
			*/
			void SetAppLogger(IAppLogger* appLogger) {
				this->appLogger = appLogger;
			}

			void StopPrograms();
			void Stop();
			void Start();
			bool Execute();
	};
}

#endif