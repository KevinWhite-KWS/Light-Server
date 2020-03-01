/*!
 * @file ICommand.h
 *
 * Defines the interface contract for a class
 * that executes a command.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _ICOMMAND_H
#define _ICOMMAND_H

#include <stdint.h>

namespace LS {
	/*!
	@brief  Interface that defines the contract for a class that executes
			a command.
	*/
	class ICommand
	{
	public:
		/*!
		  @brief   Executes the command.
		  @returns True if the command was executed successfully or
				   false if it did not execute successfully.
		*/
		virtual bool ExecuteCommand() = 0;
	};
}
#endif