#include "FlashConfigPersistance.h"

namespace LS {
	LEDConfig FlashConfigPersistance::ReadConfig() {

		LEDConfig config;
		config = led_config_storage.read();

		//byte* configBytes = dueFlashStorage.readAddress(0);
		//memcpy(config, configBytes, sizeof(LEDConfig));

		// return config->AreSettingsValid();
		return config;
	}

	void FlashConfigPersistance::SaveConfig(LEDConfig* config) {
		led_config_storage.write(*config);

		//byte configBytes[sizeof(LEDConfig)];
		//memcpy(configBytes, config, sizeof(LEDConfig));
		//dueFlashStorage.write(0, configBytes, sizeof(LEDConfig));
	}
}