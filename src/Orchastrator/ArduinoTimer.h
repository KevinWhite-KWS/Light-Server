#ifndef _ArduinoTimer_H
#define _ArduinoTimer_H

#include "Timer.h"

namespace LS {
	/*!
		@brief  Timer class that uses millis() to calcualte intervals.
		@author	Kevin White
		@date	2 Feb 21
	*/
	class ArduinoTimer : public Timer {
	protected:
		/*!
			@brief		Gets the current value of millis().
			@returns	The current value of millis() which overflows back to 0 after approx. 50 days.
			@author		Kevin White
			@date		2 Feb 21
		*/
		virtual uint32_t GetCurrent() {
			return millis();
		}
	public:
		ArduinoTimer(uint8_t interval = 50) 
			: Timer(interval) {
			// we need to initialise the first interval or otherwise the timer
			// will never return true!
			nextTime = GetCurrent() + this->interval;
		}
	};

}

#endif