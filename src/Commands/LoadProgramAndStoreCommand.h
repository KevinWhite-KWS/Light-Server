/*!
 * @file LoadProgramAndStoreCommand.h
 *
 * Handles an command that has been received
 * to load a Light Program and store it permanently
 * to flash memory so that it is loaded on next
 * restart of the device.
 *
 *
 * Written by Kevin White.
 * 27 Dec 2022
 *
 * This file is part of the LS library.
 *
 */

#ifndef _LOADPROGRAMANDSTORECOMMAND_H
#define _LOADPROGRAMANDSTORECOMMAND_H

#include "LoadProgramCommand.h"
#include "ICommand.h"
#include "../DomainInterfaces.h"
#include "../LPE/Validation/LpJsonValidator.h"
#include "../LPE/StateBuilder/LpJsonStateBuilder.h"
#include "../LPE/StateBuilder/LpJsonState.h"
#include "../ValueDomainTypes.h"
#include "../ConfigPersistance/IConfigPersistance.h"
#include "../MemoryFree.h"

namespace LS {
	/*!
	@brief  LoadProgramCommand handles a command that has been received
			to load a Light Program.
	*/
	class LoadProgramAndStoreCommand : public LoadProgramCommand
	{
	private:
		LEDConfig* ledConfig;
		IConfigPersistance* configPersistance;
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
		  @param   ledConfig			The LED configuration values
		  @param   configPersistance	The instance which permanently stores changes to the LED configuration, including the
										program loaded that is to be stored permanently.
		*/
		LoadProgramAndStoreCommand(
			ILightWebServer* lightWebServer,
			LpJsonValidator* lpValidator,
			LpJsonStateBuilder* lpStateBuilder,
			LpJsonState* lpState,
			LEDConfig* ledConfig,
			IConfigPersistance* configPersistance
		) : LoadProgramCommand(lightWebServer, lpValidator, lpStateBuilder, lpState) {

			this->ledConfig = ledConfig;
			this->configPersistance = configPersistance;
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