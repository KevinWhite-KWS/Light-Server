/*!
	@brief		Ethernet implementation of a UDP service which allows
				this server to be discovered on the local network by
				broadcasting UDP packets.
	@author		Kevin White
	@date		18 Mar 21
*/
#ifndef _EthernetUdpDiscoveryService_h
#define _EthernetUdpDiscoveryService_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "../WProgram.h"
#endif

#include "UdpDiscoveryService.h"
#include "IUdpService.h"

namespace LS {
	#define UDP_TX_PACKET_MAX_SIZE 24

	/*!
		@brief		Abstract base-class for a UDP discovery service.  The UDP
					discovery service allows clients to find this server by
					broadcasting UDP packets.
		@author		Kevin White
		@date		18 Mar 21
	*/
	class EthernetUdpDiscoveryService : public UdpDiscoveryService {
	protected:
		char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

		IUdpService *udp;

		virtual void SendDiscoveredResponse();
	public:
		EthernetUdpDiscoveryService(uint16_t port, const char* replyMsg, const char* handshakeMsg, IUdpService *udp)
			: UdpDiscoveryService(port, replyMsg, handshakeMsg) {
			this->udp = udp;
		}

		virtual void StartDiscoveryService();
		virtual void CheckForHandshake();
	};
}

#endif
