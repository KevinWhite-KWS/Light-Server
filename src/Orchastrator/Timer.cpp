#include "Timer.h"

namespace LS {
	/*!
		@brief		Constructor sets the interval the timer.
		@author		Kevin White
		@date		2 Feb 2021
	*/
	Timer::Timer(uint8_t interval) {
		if (interval < 25) {
			interval = 25;
		}

		this->interval = interval;
		// SetNext();
		// nextTime = this->GetCurrent() + interval;
		// nextTime = 0;
	}

	/*!
		@brief		Determines whether the next interval has been reached.
		@returns	True if the next interval has been reached, false if not yet reached.
		@author		Kevin White
		@date		2 Feb 2021
	*/
	bool Timer::IsTime() {
		bool isTime = GetCurrent() >= nextTime
			|| GetCurrent() < 1000;				// In case current has overflowed but nextTime is stuck at a value
												// just before overflow and thus will never be reached.

		if (isTime) {
			// set the next interval as the current one has been reached
			SetNext();
		}

		initialised = true;

		return isTime;
	}

	/*!
		@brief		Sets the next interval value.
		@author		Kevin White
		@date		2 Feb 2021
	*/
	void Timer::SetNext() {
		nextTime = GetCurrent() + interval;
	}
}