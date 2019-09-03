/*!
 * @file FixedSizeCharBuffer.h
 *
 * Encapsulates a fixed sized char buffer and provides
 * means of managing that buffer.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */
#ifndef _FIXEDSIZECHARBUFFER_h
#define _FIXEDSIZECHARBUFFER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

namespace LS {
	class FixedSizeCharBuffer {
		private:
			char* buffer;
			uint16_t bufferSize = 0;

		public:
			FixedSizeCharBuffer(uint16_t bufferSize);
			~FixedSizeCharBuffer();

			void ClearBuffer();
			char* GetBuffer();
			void LoadFromBuffer(const char* bufferToCopy);
			const uint16_t GetBufferSize();
	};
}


#endif

