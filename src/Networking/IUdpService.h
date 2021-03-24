#ifndef _UdpService_h
#define _UdpService_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "../WProgram.h"
#endif

#include <stdint.h>

namespace LS {
	struct IP {
		uint8_t firstOctet;
		uint8_t secondOctet;
		uint8_t thirdOctet;
		uint8_t fourthOctet;
	};

	class IUdpService {
	public:
		virtual uint8_t begin(uint16_t port) = 0;
		virtual int parsePacket() = 0;
		virtual int read(char* buffer, size_t len) = 0;
		virtual int beginPacket(IP ip, uint16_t port) = 0;
		virtual IP remoteIP() = 0;
		virtual uint16_t remotePort() = 0;
		virtual size_t write(const char* str) = 0;
		virtual int endPacket() = 0;
	};


}
#endif