#ifndef _IFlashConfigPersistance_H
#define _IFlashConfigPersistance_H


#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "../DomainInterfaces.h"
#include "IConfigPersistance.h"
// #include "DueFlashStorage/DueFlashStorage.h"
#include <FlashStorage_SAMD.h>

namespace LS {
	// declare this here to reserve some flash memory for storing our config values
	FlashStorage(led_config_storage, LEDConfig);

	/**
	 * @brief	Persists the LDL server settings to flash memory.
	 * @author	Kevin White
	 * @date	7 May 2021
	 */
	class FlashConfigPersistance : public IConfigPersistance {
	private:
		// DueFlashStorage dueFlashStorage;
		// FlashStorage(led_config_storage, LEDConfig);
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
		// bool ReadConfig(LEDConfig* ledConfig);
		LEDConfig ReadConfig() {
			LEDConfig config;
			config = led_config_storage.read();

			//byte* configBytes = dueFlashStorage.readAddress(0);
			//memcpy(config, configBytes, sizeof(LEDConfig));

			// return config->AreSettingsValid();
			return config;
		}

		/**
		 * @brief	Writes the LDL server settings to flash memory.
		 * @param	ledConfig		The LDL server configuration values
		 * @date	7 May 2021
		 */
		void SaveConfig(LEDConfig* ledConfig) {
			led_config_storage.write(*ledConfig);

			//byte configBytes[sizeof(LEDConfig)];
			//memcpy(configBytes, config, sizeof(LEDConfig));
			//dueFlashStorage.write(0, configBytes, sizeof(LEDConfig));
		}
	};
}

#endif
