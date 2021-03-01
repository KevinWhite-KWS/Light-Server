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
#include "../Orchastrator/IOrchastor.h"

namespace LS {
	/*!
	@brief  PowerOffCommand handles a command that has been received
			to turn-off the LEDs.
	*/
	class PowerOffCommand : public ICommand
	{
	private:
		ILightWebServer* lightWebServer;
		IOrchastor* orchastor;
		IPixelController* pixelController;
	public:
		/*!
		  @brief   Executes the command that causes the power
				   to be turned-off to the LEDs.
		  @param   lightWebServer		Pointer to the class that handles web requests.
		  @param   pixelController		Pointer to the class that interacts with the LEDs.
		  @param   orchastor		    Pointer to the orchastrating class.
		*/
		PowerOffCommand(ILightWebServer* lightWebServer, IPixelController* pixelController, IOrchastor* orchastor) {
			this->lightWebServer = lightWebServer;
			this->pixelController = pixelController;
			this->orchastor = orchastor;
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