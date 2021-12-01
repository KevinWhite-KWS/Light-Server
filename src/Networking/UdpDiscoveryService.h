
#ifndef _UdpDiscoveryService_h
#define _UdpDiscoveryService_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "../WProgram.h"
#endif

#include <stdint.h>
#include <string.h>

namespace LS {
	/*!
		@brief		Abstract base-class for a UDP discovery service.  The UDP
					discovery service allows clients to find this server by
					broadcasting UDP packets.
		@author		Kevin White
		@date		18 Mar 21
	*/
	class UdpDiscoveryService {
	protected:
		uint16_t port = 8888;
		const char* replyMsg;
		const char* handshakeMsg;

		void CheckMessageReceived(const char* msgReceived);
		virtual void SendDiscoveredResponse() = 0;
	public:
		UdpDiscoveryService(uint16_t port, const char* replyMsg, const char* handshakeMsg);

		

		virtual void StartDiscoveryService() = 0;
		virtual void CheckForHandshake() = 0;

		void SetReplyMessage(const char* response) {
			this->replyMsg = response;
		}
	};
}


#endif