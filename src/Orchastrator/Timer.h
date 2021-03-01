#ifndef _Timer_H
#define _Timer_H

#include <stdint.h>

namespace LS {
	/*!
	@brief  Abstract base class for a class that provides
			timing e.g. a timer to perform actions based
			on millis() in Arduino.
	@author	Kevin White
	@date	2 Feb 21
	*/
	class Timer
	{
	protected:
		uint8_t interval = 0;
		uint32_t nextTime = 0;
		bool initialised = false;

		virtual uint32_t GetCurrent() = 0;
		virtual void SetNext();
	public:
		Timer(uint8_t interval);
		virtual bool IsTime();
	};
}

#endif