#ifndef _IConfigPersistance_H
#define _IConfigPersistance_H


#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "..\WProgram.h"
#endif

#include "../ValueDomainTypes.h"

namespace LS {
	/**
	 * @brief	Interface which defines the contract for a class that loads and saves
	 *			server settings to non-volatile storage.
	 * @author	Kevin White
	 * @date	7 May 2021
	 */
	class IConfigPersistance {
	public:
		// virtual bool ReadConfig(LEDConfig* ledConfig);
		virtual LEDConfig ReadConfig() = 0;
		virtual void SaveConfig(LEDConfig* ledConfig) = 0;
	};
}

#endif
