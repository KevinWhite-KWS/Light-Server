#include "FlashConfigPersistance.h"

namespace LS {
	bool FlashConfigPersistance::ReadConfig(LEDConfig* config) {
		byte* configBytes = dueFlashStorage.readAddress(0);
		memcpy(config, configBytes, sizeof(LEDConfig));

		return config->AreSettingsValid();
	}

	void FlashConfigPersistance::SaveConfig(LEDConfig* config) {
		byte configBytes[sizeof(LEDConfig)];
		memcpy(configBytes, config, sizeof(LEDConfig));
		dueFlashStorage.write(0, configBytes, sizeof(LEDConfig));
	}
}