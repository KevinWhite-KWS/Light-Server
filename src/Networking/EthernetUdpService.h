#ifndef _EthernetUdpService_h
#define _EthernetUdpService_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "../WProgram.h"
#endif

#include "IUdpService.h"
#include <stdint.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

namespace LS {
	class EthernetUdpService : public IUdpService {
	protected:
		EthernetUDP udp;
	public:
		virtual uint8_t begin(uint16_t port) {
			return udp.begin(port);
		}
		virtual int parsePacket() {
			return udp.parsePacket();
		}
		virtual int read(char* buffer, size_t len) {
			return udp.read(buffer, len);
		}
		virtual int beginPacket(IP ip, uint16_t port) {
			IPAddress ipAddress(ip.firstOctet, ip.secondOctet, ip.thirdOctet, ip.fourthOctet);

			return udp.beginPacket(ipAddress, port);
		}
		virtual IP remoteIP() {
			IPAddress remoteIP = udp.remoteIP();
			IP remote;
			remote.firstOctet = remoteIP[0];
			remote.secondOctet = remoteIP[1];
			remote.thirdOctet = remoteIP[2];
			remote.fourthOctet = remoteIP[3];
			return remote;
		}
		virtual uint16_t remotePort() {
			return udp.remotePort();
		}
		virtual size_t write(const char* str) {
			return udp.write(str);
		}
		virtual int endPacket() {
			return udp.endPacket();
		}
	};
}
#endif
