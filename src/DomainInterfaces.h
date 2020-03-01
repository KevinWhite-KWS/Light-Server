#ifndef _DOMAININTERFACES_H
#define _DOMAININTERFACES_H


#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <stdint.h>

namespace LS {
	/*
	@brief	Interface that defines hte contract for a class that provides application logging functionality.
	*/
	class IAppLogger {
		public:
			virtual void logEvent(uint32_t start, uint8_t level, const char* event, const char* trigger, uint32_t end = 0, const char* msg = nullptr) = 0;
	};

	/*!
	@brief  Interface that defines the contract for a class that controls a set of LEDs.
	*/
	class IPixelController
	{
		public:
			IPixelController() { }
			virtual void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) = 0;
			virtual void show() = 0;
			virtual void fill(uint32_t c = 0, uint16_t first = 0, uint16_t count = 0) = 0;
			virtual uint16_t numPixels(void) const = 0;
			virtual uint32_t getPixelColor(uint16_t n) const = 0;
	};


	class ILightWebServer;

	/*!
	  @brief	CommandType provides a representation of the type of command
				that has been received by a LightServer.  This may be an invalid
				command.
	*/
	enum CommandType { 
		NONE, 
		NOAUTH,			// Not authorised (missing valid basic auth header)
		INVALID,		// Not a recognised / valid request
		LOADPROGRAM,	// Attmpt to load an LP
		POWEROFF,		// Turn off all LEDs
		POWERON,		// Turn on all LEDs to white unless an explicit colour has been specified
		CHECKPOWER,		// Returns the state of the LEDS (whether any are curently on or not)
		GETABOUT,		// Returns information about the server (versions and stuff)
		SETLEDS			// Sets the number of connected LEDs
	};

	/*!
	@brief  Interface that defines the contract for a class that provides
			web server functionality such as allowing requests for specific
			resources to be handled.

			NOTE: this has been created from the WebServer.h implementation
			in order to decouple the concrete implementation and allowing
			unit testing to be performed.
	*/
	class IWebServer
	{
		public:
			// passed to a command to indicate what kind of request was received
			enum ConnectionType { INVALID, GET, HEAD, POST, PUT, DELETE, PATCH, OPTIONS /* CORS */ };

			// any commands registered with the web server have to follow
			// this prototype.
			// url_tail contains the part of the URL that wasn't matched against
			//          the registered command table.
			// tail_complete is true if the complete URL fit in url_tail,  false if
			//          part of it was lost because the buffer was too small.
			typedef void Command(ILightWebServer* lightWebServer, IWebServer& server, ConnectionType type,
				char* url_tail, bool tail_complete);

			// Prototype for the optional function which consumes the URL path itself.
			// url_path contains pointers to the seperate parts of the URL path where '/'
			//          was used as the delimiter.
			typedef void UrlPathCommand(IWebServer& server, ConnectionType type,
				char** url_path, char* url_tail,
				bool tail_complete);


			IWebServer() { }
			virtual void begin() = 0;
			virtual void setDefaultCommand(Command* cmd) = 0;
			virtual void setFailureCommand(Command* cmd) = 0;
			virtual void httpFail() = 0;
			virtual void httpNoContent() = 0;
			virtual void httpSuccess(const char* contentType = "text/html; charset=utf-8",
				const char* extraHeaders = nullptr) = 0;
			virtual void httpUnauthorized() = 0;
			virtual int read() = 0;
			virtual void flushBuf() = 0;
			virtual void addCommand(const char* verb, Command* cmd) = 0;
			virtual void processConnection(char* buff, int* bufflen) = 0;
			virtual void setLightWebServer(ILightWebServer* lightWebServer) = 0;
			virtual void closeConnection() = 0;
			virtual void printP(const char* str) = 0;
			virtual bool checkCredentials(const char authCredentials[45]) = 0;
	};

	/*!
	@brief  Interface that defines the contract for a class that provides a Restful
			web server.

			NOTE: the method SetCommandType breaks encapsulation BUT it does provide
			a convenient means of packing web handlers into the same class.  The web handlers
			are protected static methods as these callbacks cannot be apart of the class.  Thus,
			they require external access to the method that sets the command type which
			is determined by the handler that was invoked.
	*/
	class ILightWebServer {
		public:
			/*!
			  @brief	Sets the type of command that was executed.
			  @param	commandType		The type of command that was executed.
			*/
			virtual void SetCommandType(CommandType commandType) = 0;

			/*!
			  @brief	Gets a reference to the temporary buffer that can be used
						to load the web request content.
			  @param	clearBuffer		True to clear the buffer before returning or
										false to leave the buffer as it is.
			  @returns	char*			A pointer to the loading buffer.
			*/
			virtual char* GetLoadingBuffer(bool clearBuffer = true) = 0;

			/*!
			  @brief	Gets a reference to the buffer that contains the Base64 encoded
						basic credentials to be used for authenticating requests.
			  @returns	char*			A pointer to the Base64 encoded user credentials.
			*/
			virtual const char* GetAuthCredentials() = 0;

			/*!
			@brief		Closes the current connection and responds with a HTTP ERROR.
			*/
			virtual void RespondError() = 0;

			/*!
			@brief		Closes the current connection and responds with a HTTP NOT AUTHORISED
			*/
			virtual void RespondNotAuthorised() = 0;

			/*!
			@brief		Closes the current connection and respons with a HTTP NO CONTENT (204).
			*/
			virtual void RespondNoContent() = 0;

			/*!
			@brief		Closes the current connection and respons with a HTTP OK (200).
			@param		str			Optional pointer to a buffer that contains the text to be sent in the body.
			*/
			virtual void RespondOK(const char* str) = 0;

			/*!
			@brief		Checks whether a new command has been received and returns the command type.
			@returns	CommandType		The type of command that has been received (if any).
			*/
			virtual CommandType HandleNextCommand() = 0;
	};
}

#endif
