#include "EthernetUdpDiscoveryService.h"

namespace LS {
	/*!
		@brief	Starts the UDP ether discovery service.
		@author	Kevin White
		@date	18 Mar 21
	*/
	void EthernetUdpDiscoveryService::StartDiscoveryService() {
		udp->begin(port);
	}

	/*!
		@brief	Checks whether a UDP handshake message has been received or not.
		@author	Kevin White
		@date	18 Mar 21
	*/
	void EthernetUdpDiscoveryService::CheckForHandshake() {
		int packetSize = udp->parsePacket();
		if (packetSize) {
			// read the packet into packetBufffer
			udp->read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
			CheckMessageReceived(packetBuffer);
		}
	}

	/*!
		@brief	A handshake message has been received so this method sends back
				the expected response to let the client know that we are available.
		@author	Kevin White
		@date	18 Mar 21
	*/
	void EthernetUdpDiscoveryService::SendDiscoveredResponse() {
		// somehow has made contact so respond with
		// a reply that lets them know we are here and waiting!
		udp->beginPacket(udp->remoteIP(), udp->remotePort());
		udp->write(replyMsg);
		udp->endPacket();

		for (int i = 0; i < UDP_TX_PACKET_MAX_SIZE; i++) {
			packetBuffer[i] = 0x00;
		}
	}
}