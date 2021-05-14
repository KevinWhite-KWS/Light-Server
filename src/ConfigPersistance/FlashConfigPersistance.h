#ifndef _IFlashConfigPersistance_H
#define _IFlashConfigPersistance_H


#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "../DomainInterfaces.h"
#include "IConfigPersistance.h"
#include "DueFlashStorage/DueFlashStorage.h"

namespace LS {
	/**
	 * @brief	Persists the LDL server settings to flash memory.
	 * @author	Kevin White
	 * @date	7 May 2021
	 */
	class FlashConfigPersistance : public IConfigPersistance {
	private:
		DueFlashStorage dueFlashStorage;
	public:
		/**
		 * @brief	Reads the LDL server settings from flash memory.
		 * @param	ledConfig		The LDL server configuration value
		 * @returns	True if the LDL server settings were read successfully.  False if
		 *			they are not valid.  The might not be valid if no settings have yet
		 *			been persisted in which case reasonable default values should be used.
		 * @author	Kevin White
		 * @date	7 May 2021
		 */
		bool ReadConfig(LEDConfig* ledConfig);

		/**
		 * @brief	Writes the LDL server settings to flash memory.
		 * @param	ledConfig		The LDL server configuration values
		 * @date	7 May 2021
		 */
		void SaveConfig(LEDConfig* ledConfig);
	};
}

#endif
