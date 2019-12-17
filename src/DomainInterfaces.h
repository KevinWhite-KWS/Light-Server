#ifndef _DOMAININTERFACES_H
#define _DOMAININTERFACES_H

namespace LS {
	/*!
	@brief  Interface that defines the contract for a class that controls a set of LEDs.
	*/
	class IPixelController
	{
		public:
			IPixelController() { }
			virtual void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) = 0;
			virtual void show() = 0;
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
		INVALID, 
		LOADPROGRAM 
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
			enum ConnectionType { INVALID, GET, HEAD, POST, PUT, DELETE, PATCH };

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
			virtual void httpSuccess(const char* contentType = "text/html; charset=utf-8",
				const char* extraHeaders = NULL) = 0;
			virtual int read() = 0;
			virtual void flushBuf() = 0;
			virtual void addCommand(const char* verb, Command* cmd) = 0;
			virtual void processConnection(char* buff, int* bufflen) = 0;
			virtual void setLightWebServer(ILightWebServer* lightWebServer) = 0;
			virtual void closeConnection() = 0;
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
	};
}

#endif
