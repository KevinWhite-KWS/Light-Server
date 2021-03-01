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
#include "../Orchastrator/IOrchastor.h"
#include "../StringProcessor.h"

namespace LS {
	/*!
	@brief  PowerOnCommand handles a command that has been received
			to turn-on the LEDs.  If a colour has been specified
			then the LEDs are set to that colour.  Otherwise, the
			LEDs are set to white.
	*/
	class PowerOnCommand : public ICommand
	{
	private:
		ILightWebServer* lightWebServer;
		IOrchastor* orchastor;
		IPixelController* pixelController;
		StringProcessor* stringProcessor;
	public:
		/*!
		  @brief   Executes the command that causes the power
				   to be turned-on to the LEDs.
		  @param   lightWebServer		Pointer to the class that handles web requests.
		  @param   pixelController		Pointer to the class that interacts with the LEDs.
		  @param   orchastor		    Pointer to the orchastrating class.
		*/
		PowerOnCommand(ILightWebServer* lightWebServer, IPixelController* pixelController, IOrchastor* orchastor, StringProcessor* stringProcessor) {
			this->lightWebServer = lightWebServer;
			this->pixelController = pixelController;
			this->orchastor = orchastor;
			this->stringProcessor = stringProcessor;
		}

		/*!
		  @brief   Executes the command that causes the power
				   to be turned-on to the LEDs.
		  @returns True if the command was executed successfully or
				   false if it did not execute successfully.
		*/
		bool ExecuteCommand();

		/*!
			@brief   Convert separate red, green and blue values into a single
					 "packed" 32-bit RGB color.
			@param   r  Red brightness, 0 to 255.
			@param   g  Green brightness, 0 to 255.
			@param   b  Blue brightness, 0 to 255.
			@return  32-bit packed RGB value, which can then be assigned to a
					 variable for later use or passed to the setPixelColor()
					 function. Packed RGB format is predictable, regardless of
					 LED strand color order.
		  */
		static uint32_t   PackColor(uint8_t r, uint8_t g, uint8_t b) {
			return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
		}
	};
}
#endif