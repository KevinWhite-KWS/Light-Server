#include "LpJsonState.h"

namespace LS {
	/*!
		@brief		Gets a pointer to the light program's source JSON
					document.
		@returns	A pointer to the source JSON document.
		@author		Kevin White
		@date		21 Dec 2020
	*/
	StaticJsonDocument<BUFFER_LP>* LpJsonState::getLpJsonDoc() {
		return &lpJsonDoc;
	}
}