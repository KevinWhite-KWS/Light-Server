/*!
 * @file PowerOffCommand.h
 *
 * Handles an command that has been received
 * to turn-off the power to the LEDs.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _POWEROFFCOMMAND_H
#define _POWEROFFCOMMAND_H

#include "ICommand.h"
#include "../DomainInterfaces.h"
#include "../LPE.h"

namespace LS {
	/*!
	@brief  PowerOffCommand handles a command that has been received
			to turn-off the LEDs.
	*/
	class PowerOffCommand : public ICommand
	{
	private:
		ILightWebServer* lightWebServer;
		IPixelController* pixelController;
		LPE* lpe;
	public:
		/*!
		  @brief   Executes the command that causes the power
				   to be turned-off to the LEDs.
		  @param   lightWebServer		Pointer to the class that handles web requests.
		  @param   pixelController		Pointer to the class that interacts with the LEDs.
		  @param   lpe				    Pointer to the Light Program Executor instance that handles
										execution of Light Programs.
		*/
		PowerOffCommand(ILightWebServer* lightWebServer, IPixelController* pixelController, LPE* lpe) {
			this->lightWebServer = lightWebServer;
			this->pixelController = pixelController;
			this->lpe = lpe;
		}

		/*!
		  @brief   Executes the command that causes the power
				   to be turned-off to the LEDs.
		  @returns True if the command was executed successfully or
				   false if it did not execute successfully.
		*/
		bool ExecuteCommand();
	};
}
#endif