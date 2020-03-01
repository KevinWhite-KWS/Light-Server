/*!
 * @file PowerOffCommand.h
 *
 * Handles an command that has been received
 * to turn-on the power to the LEDs.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _POWERONCOMMAND_H
#define _POWERONCOMMAND_H

#include "ICommand.h"
#include "../DomainInterfaces.h"
#include "../LPE.h"
#include "../Adafruit_NeoPixel.h"

namespace LS {
	/*!
	@brief  PowerOnCommand handles a command that has been received
			to turn-on the LEDs.
	*/
	class PowerOnCommand : public ICommand
	{
	private:
		ILightWebServer* lightWebServer;
		IPixelController* pixelController;
		LPE* lpe;
		StringProcessor* stringProcessor;
	public:
		/*!
		  @brief   Executes the command that causes the power
				   to be turned-on to the LEDs.
		  @param   lightWebServer		Pointer to the class that handles web requests.
		  @param   pixelController		Pointer to the class that interacts with the LEDs.
		  @param   lpe				    Pointer to the Light Program Executor instance that handles
									    execution of Light Programs.
		  @param   stringProcessor		Pointer to the class that provides perform string parsing.
		*/
		PowerOnCommand(ILightWebServer* lightWebServer, IPixelController* pixelController, LPE* lpe, StringProcessor* stringProcessor) {
			this->lightWebServer = lightWebServer;
			this->pixelController = pixelController;
			this->lpe = lpe;
			this->stringProcessor = stringProcessor;
		}

		/*!
		  @brief   Executes the command that causes the power
				   to be turned-on to the LEDs.
		  @returns True if the command was executed successfully or
				   false if it did not execute successfully.
		*/
		bool ExecuteCommand();
	};
}
#endif