/*!
 * @file GetAboutCommand.h
 *
 * Handles an command that has been received
 * get information about the server.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _GETABOUTCOMMAND_H
#define _GETABOUTCOMMAND_H

#include "ICommand.h"
#include "../DomainInterfaces.h"
#include "../LPE.h"
#include "../ArduinoJson-v6.17.2.h"

#define		LS_VERSION			"1.0.0"
#define		LDL_VERSION			"1.0.0"
#define		NUMLEDS	19

namespace LS {
	/*!
	@brief  PowerOnCommand handles a command that has been
	received to get information about the server.
	*/
	class GetAboutCommand : public ICommand
	{
	private:
		ILightWebServer* lightWebServer;
		StaticJsonDocument<1000>* webDoc;
		FixedSizeCharBuffer* webResponse;
		LEDConfig* ledConfig;
	public:
		/*!
		  @brief   Executes the command that gets information
				   the server.
		  @param   lightWebServer		Pointer to the class that handles web requests.
		  @param   webDoc				Pointer to the Arduino JSON document that is used to construct the JSON web response.
		  @param   webResponse			Pointer to the buffer that stores the HTTP reponse.
		  @param   ledConfig			Pointer to the class that contains the LED configuration details.
		*/
		GetAboutCommand(ILightWebServer* lightWebServer, StaticJsonDocument<1000>* webDoc, FixedSizeCharBuffer* webResponse, LEDConfig* ledConfig) {
			this->lightWebServer = lightWebServer;
			this->webDoc = webDoc;
			this->webResponse = webResponse;
			this->ledConfig = ledConfig;
		}

		/*!
		  @brief   Executes the command that gets information
				   about the server.
		  @returns True if the command was executed successfully or
				   false if it did not execute successfully.
		*/
		bool ExecuteCommand();
	};
}
#endif