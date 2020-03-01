/*!
 * @file NoAuthCommand.h
 *
 * Handles an command that has been received but where the
 * request could not be authenticated.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _NOAUTHCOMMAND_H
#define _NOAUTHCOMMAND_H

#include "ICommand.h"
#include "../DomainInterfaces.h"

namespace LS {
	/*!
	@brief  NoAuthCommand handles an invalid command that has been received
			but cannot be authenticated.
	*/
	class NoAuthCommand : public ICommand
	{
	private:
		ILightWebServer* lightWebServer;
	public:
		/*!
		  @brief   Executes the command and causes the web connection
				   to be closed with a HTTP 401 response code.
		  @param   lightWebServer		Pointer to the class that handles web requests.
		*/
		NoAuthCommand(ILightWebServer* lightWebServer) {
			this->lightWebServer = lightWebServer;
		}

		/*!
		  @brief   Executes the command and causes the web connection
				   to be closed with a HTTP 401 response code.
		  @returns True if the command was executed successfully or
				   false if it did not execute successfully.
		*/
		bool ExecuteCommand();
	};
}
#endif