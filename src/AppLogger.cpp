#include "AppLogger.h"

namespace LS {
	/*!
	  @brief   Logs an event that has occurred, such as the start of the execution
			   cycle.  Events are logged in the following standard format:
			   [event]\t\t[trigger]\t\t[elapsed time in ms]\t[start time in ms]\t[end time in ms]\t[free memory]\t[additonal msg]
	  @param   start   The start time of the event in milliseconds
	  @param   level   The 'level' of the event i.e. embedding level
	  @param   event   The name of the event i.e. cycle for the execution cycle
	  @param   trigger The trigger of the event i.e. started
	  @param   end     The end time of the event in milliseconds (can be 0)
	  @param   msg     Any additional message relevant to the event (can be null)
	*/
	void AppLogger::logEvent(uint32_t start, uint8_t level, const char* event, const char* trigger, bool isStartEvent, uint32_t end, const char* msg) {
		uint32_t duration = (end > 0 && start > 0 ? end - start : 0);

		// IF SERIAL PRODUCES NOTHING IN THE #ifndef DISABLE_LOGGING BLOCK BELOW THEN SIMPLY ENABLE THIS
		// WHICH DUPLICATES IT
		/*char indent[20];
		for (int i = 0; i < level; i++) { indent[i] = '.'; }
		indent[level] = 0;

		Serial.print(indent);
		Serial.print("[");
		Serial.print(event);
		Serial.print("]\t\t");
		Serial.print("[");
		Serial.print((isStartEvent) ? "START" : "END");
		Serial.print("]\t\t");
		Serial.print("[");
		Serial.print(trigger);
		Serial.print("]\t\t");
		Serial.print("[");
		Serial.print(duration);
		Serial.print("ms]\t");
		Serial.print("[");
		Serial.print(start);
		Serial.print("]\t");
		Serial.print("[");
		Serial.print(end);
		Serial.print("]\t");
		Serial.print("[");
		Serial.print(freeMemory());
		Serial.print(" bytes]\t");
		Serial.print("[");
		Serial.print((msg == nullptr ? "" : msg));
		Serial.println("]");*/

#ifndef DISABLE_LOGGING
		char indent[20];
		for (int i = 0; i < level; i++) { indent[i] = '.'; }
		indent[level] = 0;

		Log.verbose("%s[%s]\t\t[%s]\t\t[%s]\t\t[%d ms]\t[%d]\t[%d]\t[%d bytes]\t[%s]\r\n",
			indent,
			event,
			(isStartEvent ? "START" : "END"),
			trigger,
			duration,
			start,
			end,
			// TEMP
			freeMemory(),
			(msg == nullptr ? "" : msg)
		);
#endif
	}
}