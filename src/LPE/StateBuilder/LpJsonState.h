#ifndef _LpJsonState_h
#define _LpJsonState_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

#include "LpState.h"
#include "..\..\ValueDomainTypes.h"
#include "..\..\ArduinoJson-v6.17.2.h"

namespace LS {
	/*!
	@brief	Stores the data necessary to track the state of a Light Program
			whilst it is being executed and also the JSON document from
			which the LP has been parsed.  This state allocates the memory
			necessary to track a light program as it executes and also
			reference LPI strings from the source JSON document.  Alternatively,
			we could copy the strings from the JSON document but then
			we'd have to pre-allocate memory to track those or otherwise
			face possible heap allocation issues.
	@author	Kevin White
	@date	21 Dec 2020
	*/
	class LpJsonState : public LpState {
		private:
			// 2000: *** BUFFER ALLOCATION *** - Store an entire LP state
			StaticJsonDocument<BUFFER_LP> lpJsonDoc;

		public:
			StaticJsonDocument<BUFFER_LP>* getLpJsonDoc();
	};
}
#endif