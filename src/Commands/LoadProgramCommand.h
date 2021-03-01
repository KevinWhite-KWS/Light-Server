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
#include "../LPE/Validation/LpJsonValidator.h"
#include "../LPE/StateBuilder/LpJsonStateBuilder.h"
#include "../LPE/StateBuilder/LpJsonState.h"

namespace LS {
	/*!
	@brief  LoadProgramCommand handles a command that has been received
			to load a Light Program.
	*/
	class LoadProgramCommand : public ICommand
	{
	private:
		ILightWebServer* lightWebServer;
		FixedSizeCharBuffer* lpBuffer;
		LpJsonValidator* lpValidator;
		LpJsonStateBuilder* lpStateBuilder;
		LpJsonState* lpState;
	protected:
		LPValidateResult validateResult;
	public:
		/*!
		  @brief   Executes the command that cause a Light Program to be loaded.
		  @param   lightWebServer		Pointer to the class that handles web requests.
		  @param   lpBuffer				Pointer to the buffer that stores the Light Program
										to be loaded.
		  @param   lpValidator			Pointer to the class that validates Light Programs before they are loaded.
		  @param   lpStateBuilder		Pointer to the class that builds a tree represents of a Light Program which
										can then be executed.
		  @param   lpState				Pointer to the class that stores the tree representations of a Light Program.
		*/
		LoadProgramCommand(ILightWebServer* lightWebServer, 
			FixedSizeCharBuffer* lpBuffer, 
			LpJsonValidator* lpValidator, 
			LpJsonStateBuilder* lpStateBuilder, 
			LpJsonState* lpState) {

			this->lightWebServer = lightWebServer;
			this->lpBuffer = lpBuffer;
			this->lpValidator = lpValidator;
			this->lpStateBuilder = lpStateBuilder;
			this->lpState = lpState;
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