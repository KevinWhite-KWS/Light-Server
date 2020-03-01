/*!
 * @file InvalidCommand.h
 *
 * Handles an command that has been received that is invalid.
 * Example: an invalid web erequest to non-existent API endpoint.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _INVALIDCOMMAND_H
#define _INVALIDCOMMAND_H

#include "ICommand.h"
#include "../DomainInterfaces.h"

namespace LS {
	/*!
	@brief  InvalidCommand handles an invalid command that has been received
			by the server.
	*/
	class InvalidCommand : public ICommand
	{
	private:
		ILightWebServer* lightWebServer;
	public:
		/*!
		  @brief   Executes the command and causes the web connection
				   to be closed with a HTTP 400 response code.
		  @param   lightWebServer		Pointer to the class that handles web requests.
		*/
		InvalidCommand(ILightWebServer* lightWebServer) {
			this->lightWebServer = lightWebServer;
		}

		/*!
		  @brief   Executes the command and causes the web connection
				   to be closed with a HTTP 400 response code.
		  @returns True if the command was executed successfully or
				   false if it did not execute successfully.
		*/
		bool ExecuteCommand();
	};
}
#endif