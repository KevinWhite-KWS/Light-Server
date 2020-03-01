/*!
 * @file SetLedsCommand.h
 *
 * Handles an command that has been received
 * to set the configuration of the connected leds.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _SETLEDSCOMMAND_H
#define _SETLEDSCOMMAND_H

#include "ICommand.h"
#include "../DomainInterfaces.h"
#include "../LPE.h"

namespace LS {
	/*!
	@brief  PowerOnCommand handles a command that has been
	received to set the configuration of the connected leds.
	*/
	class SetLedsCommand : public ICommand
	{
	private:
		ILightWebServer* lightWebServer;
		StringProcessor* stringProcessor;
	public:
		/*!
		  @brief   Executes the command to set the configuration of the connected leds.
		  @param   lightWebServer		Pointer to the class that handles web requests.
		  @param   stringProcessor		Pointer to the class that provides string parsing functionality.
		*/
		SetLedsCommand(ILightWebServer* lightWebServer, StringProcessor* stringProcessor) {
			this->lightWebServer = lightWebServer;
			this->stringProcessor = stringProcessor;
		}

		/*!
		  @brief   Executes the command to set the configuration of the connected leds.
		  @returns True if the command was executed successfully or
				   false if it did not execute successfully.
		*/
		bool ExecuteCommand();
	};
}
#endif