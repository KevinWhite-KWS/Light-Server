/*!
 * @file CheckPowerCommand.h
 *
 * Handles a command to retrieve
 * the power status of the LEDs
 * (that is: are any turned on ?).
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _CHECKPOWERCOMMAND_H
#define _CHECKPOWERCOMMAND_H

#include "ICommand.h"
#include "../DomainInterfaces.h"
#include "../ArduinoJson-v6.17.2.h"
#include "../FixedSizeCharBuffer.h"
#include "../ValueDomainTypes.h"

namespace LS {
	/*!
	@brief  CheckPowerCommand handles a command that has been
			received in order to check the power status
			of the LEDs.
	*/
	class CheckPowerCommand : public ICommand
	{
	private:
		ILightWebServer* lightWebServer;
		IPixelController* pixelController;
		StaticJsonDocument<BUFFER_JSON_RESPONSE_SIZE>* webDoc;
		FixedSizeCharBuffer* webResponse;
	public:
		/*!
		  @brief   Constructor injects the dependencies.
		  @param   lightWebServer		Pointer to the class that handles web requests.
		  @param   pixelController		Pointer to the class that interacts with the LEDs.
		  @param   webResponseDoc		Pointer to the Arduino JSON document that is used to construct the JSON web response.
		*/
		CheckPowerCommand(ILightWebServer* lightWebServer, IPixelController* pixelController, StaticJsonDocument<BUFFER_JSON_RESPONSE_SIZE>* webDoc, FixedSizeCharBuffer* webResponse) {
			this->lightWebServer = lightWebServer;
			this->pixelController = pixelController;
			this->webDoc = webDoc;
			this->webResponse = webResponse;
		}

		/*!
		  @brief   Executes the command that checks the
				   power status of the LEDs.
		  @returns True if the command was executed successfully or
				   false if it did not execute successfully.
		*/
		bool ExecuteCommand();
	};
}
#endif