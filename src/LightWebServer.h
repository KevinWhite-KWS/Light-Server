/*!
 * @file LightWebServer.h
 *
 * Concrete web server providing a Restful
 * interface to the outside.
 *
 *
 * Written by Kevin White.
 * 16th Dec 2019
 *
 * This file is part of the LS library.
 *
 */

#ifndef _LIGHTWEBSERVER_h
#define _LIGHTWEBSERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DomainInterfaces.h"
#include "FixedSizeCharBuffer.h"

namespace LS {
	/*!
	@brief  Provides a concrete Restful interface for interacting
			with the Light Server.
	*/
	class LightWebServer : public ILightWebServer {
		
		protected:
			IWebServer* webServer;
			FixedSizeCharBuffer* loadingBuffer;
			CommandType currentCommand;
			const char* authCredentials;				// Base64 encoded authentication credentials

			/*!
			 @brief		Checks the authentication header against the stored credentials.
			 @param		lightWebServer	A pointer to this LightWebServer instance.  Required as the handler has to be a static method.
			 @param		server			A pointer to the web server.
			 @returns	bool			True if the authentication credentials are valid, false otherwise.
			*/
			static bool CheckAuth(ILightWebServer* lightWebServer, IWebServer& server);

			/*!
			@brief	Loads the loading buffer from the body of the incoming web server request.
			@param	lightWebServer		A pointer to this LightWebServer instance.  Required as the handler has to be a static method.
			@param	server				A pointer to the web server.
			*/
			static void LoadBody(ILightWebServer* lightWebServer, IWebServer& server);

			/*!
			@brief  Handles an invalid web command.  Sets the web server status to "Invalid".
			@param	lightWebServer		A pointer to this LightWebServer instance.  Required as the handler has to be a static method.
			@param	server				A pointer to the web server.
			@param	type				The verb of the connection or INVALID for an invalid request.
			@param	header				A pointer to the header.
			@param	tailComplete		True if the tail is complete
			*/
			static void HandleCommandInvalid(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char* head, bool tailComplete);
			/*!
			@brief  Handles a request to load a new LP.  Sets the web server status to "LOADPROGRAM" and loads
					the new program into the loading buffer.
			@param	lightWebServer		A pointer to this LightWebServer instance.  Required as the handler has to be a static method.
			@param	server				A pointer to the web server.
			@param	type				The verb of the connection or INVALID for an invalid request.
			@param	header				A pointer to the header.
			@param	tailComplete		True if the tail is complete
			*/
			static void HandleCommandLoadProgram(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char* head, bool tailComplete);
			/*!
			@brief  Handles a request to turn-off the LEDS.  Sets the web server status to "POWEROFF".
			@param	lightWebServer		A pointer to this LightWebServer instance.  Required as the handler has to be a static method.
			@param	server				A pointer to the web server.
			@param	type				The verb of the connection or INVALID for an invalid request.
			@param	header				A pointer to the header.
			@param	tailComplete		True if the tail is complete
			*/
			static void HandleCommandPowerOff(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char* head, bool tailComplete);
			/*!
			@brief  Handles a request to turn-on the LEDS to a specific colour (or white if no valid colour has been specified.  
				    Sets the web server status to "POWERON".
			@param	lightWebServer		A pointer to this LightWebServer instance.  Required as the handler has to be a static method.
			@param	server				A pointer to the web server.
			@param	type				The verb of the connection or INVALID for an invalid request.
			@param	header				A pointer to the header.
			@param	tailComplete		True if the tail is complete
			*/
			static void HandleCommandPowerOn(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char* head, bool tailComplete);
			/*!
			@brief  Handles a request to GET the state of the LEDS (whether any are presently on or not).  Sets the web server status to "CHECKPOWER".
			@param	lightWebServer		A pointer to this LightWebServer instance.  Required as the handler has to be a static method.
			@param	server				A pointer to the web server.
			@param	type				The verb of the connection or INVALID for an invalid request.
			@param	header				A pointer to the header.
			@param	tailComplete		True if the tail is complete
			*/
			static void HandleCommandCheckPower(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char* head, bool tailComplete);
			/*!
			@brief  Handles a request to GET the current configuration values.  Sets the web server status to "GETCONFIG".
			@param	lightWebServer		A pointer to this LightWebServer instance.  Required as the handler has to be a static method.
			@param	server				A pointer to the web server.
			@param	type				The verb of the connection or INVALID for an invalid request.
			@param	header				A pointer to the header.
			@param	tailComplete		True if the tail is complete
			*/
			static void HandleCommandGetAbout(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char* head, bool tailComplete);
			/*!
			@brief  Handles a request to set the number of connected LEDS.  Sets the web server status to "SETLEDS".
			@param	lightWebServer		A pointer to this LightWebServer instance.  Required as the handler has to be a static method.
			@param	server				A pointer to the web server.
			@param	type				The verb of the connection or INVALID for an invalid request.
			@param	header				A pointer to the header.
			@param	tailComplete		True if the tail is complete
			*/
			static void HandleCommandSetLeds(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char* head, bool tailComplete);
		public:
			/*!
			@brief  Default constructor sets references to the mandatory properties.
			@param	webServer			A pointer to the web server.
			@param	loadingBuffer		A pointer to the buffer that is used for loading received requests.
			*/
			LightWebServer(IWebServer* webServer, FixedSizeCharBuffer* loadingBuffer, char *basicAuthCredentials);

			/*!
			@brief  Starts the web server so that connections can be processed.
			*/
			void Start();

			/*!
			@brief  Sets the type of command that has been received.  This is required - even though it breaks encapsulation -
					as the handlers have to be static methods.
			@param	commandType			The type of command that has been received by the server.
			*/
			void SetCommandType(CommandType commandType);

			/*!
			@brief		Gets the current command type.
			@returns	CommandType		The type of command that has been received.
			*/
			CommandType GetCommandType();

			/*!
			@brief  Gets a reference to the buffer that is used to load the request.
			@param	clearBuffer			True to clear the buffer when retriving or false to leave the buffer as is.
			@returns	char*			A reference to the loading buffer.
			*/
			char* GetLoadingBuffer(bool clearBuffer = true);


			/*!
				@brief		Gets a reference to the buffer that is used to load the body.
				@returns	FixedSizeCharBuffer*	A reference to the body buffer.
				@author		Kevin White
				@date		14 July 2021
			*/
			FixedSizeCharBuffer* GetLoadingFixedSizeBuffer();

			/*!
			  @brief	Gets a reference to the buffer that contains the Base64 encoded
						basic credentials to be used for authenticating requests.
			  @returns	char*			A pointer to the Base64 encoded user credentials.
			*/
			const char* GetAuthCredentials() {
				return authCredentials;
			}

			/*!
			@brief		Checks whether a new command has been received and returns the command type.
			@returns	CommandType		The type of command that has been received (if any).
			*/
			CommandType HandleNextCommand();

			/*!
			@brief		Closes the current connection and responds with a HTTP OK.
			@param		str				The body response to be writtern (if any).
			*/
			void RespondOK(const char* str = nullptr);

			/*!
			@brief		Closes the current connection and respons with a HTTP NO CONTENT (204).
			*/
			void RespondNoContent();

			/*!
			@brief		Closes the current connection and responds with a HTTP ERROR.
			*/
			void RespondError();

			/*!
			@brief		Closes the current connection and responds with a HTTP NOT AUTHORISED
			*/
			void RespondNotAuthorised();
	};
}
#endif