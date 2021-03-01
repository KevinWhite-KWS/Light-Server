#ifndef _InstructionType_h
#define _InstructionType_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "..\..\WProgram.h"
#endif

namespace LS {
	enum InstructionType {
		Lpi,
		Repeat
	};
}

#endif