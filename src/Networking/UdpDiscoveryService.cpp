#include "UdpDiscoveryService.h"

namespace LS {
	/*!
		@brief	Constructor injects configuration values of the discovering service.
		@param	port			The port on which to listen for incoming UDP packets.
		@param	replyMsg		The message to reply back with when a UDP packet has been recieved.
		@param	handshakeMsg	The message that must be received in order to trigger the replyMsg
								to be returned.
		@author	Kevin White
		@date	18 Mar 21
	*/
	UdpDiscoveryService::UdpDiscoveryService(uint16_t port, const char* replyMsg, const char* handshakeMsg) {
		this->port = port;
		this->replyMsg = replyMsg;
		this->handshakeMsg = handshakeMsg;
	}

	/*!
		@brief	Checks whether a message that has been recieved matches the
				handshake message.  If it does then the concrete class
				is asked to send a response.
		@author	Kevin White
		@date	18 Mar 21
	*/
	void UdpDiscoveryService::CheckMessageReceived(const char* msgReceived) {
		if (msgReceived == nullptr) {
			return;
		}

		if (strcmp(handshakeMsg, msgReceived) == 0) {
			// handshake msg received! - send back a response
			SendDiscoveredResponse();
		}
	}
}