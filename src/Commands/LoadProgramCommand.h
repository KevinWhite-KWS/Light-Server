/*!
 * @file LoadProgramCommand.h
 *
 * Handles an command that has been received
 * to load a Light Program.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _LOADPROGRAMCOMMAND_H
#define _LOADPROGRAMCOMMAND_H

#include "ICommand.h"
#include "../DomainInterfaces.h"
#include "../LPE.h"

namespace LS {
	/*!
	@brief  LoadProgramCommand handles a command that has been received
			to load a Light Program.
	*/
	class LoadProgramCommand : public ICommand
	{
	private:
		ILightWebServer* lightWebServer;
		LPE* lpe;
		FixedSizeCharBuffer* lpBuffer;
	public:
		/*!
		  @brief   Executes the command that cause a Light Program to be loaded.
		  @param   lightWebServer		Pointer to the class that handles web requests.
		  @param   lpe				    Pointer to the Light Program Executor instance that handles
										execution of Light Programs.
		  @param   lpBuffer				Pointer to the buffer that stores the Light Program
										to be loaded.
		*/
		LoadProgramCommand(ILightWebServer* lightWebServer, LPE* lpe, FixedSizeCharBuffer* lpBuffer) {
			this->lightWebServer = lightWebServer;
			this->lpe = lpe;
			this->lpBuffer = lpBuffer;
		}

		/*!
		  @brief   Executes the command that causes a Light
				   Program to be loaded.
		  @returns True if the command was executed successfully or
				   false if it did not execute successfully.
		*/
		bool ExecuteCommand();
	};
}
#endif