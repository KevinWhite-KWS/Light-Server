/*
 Name:		Light_Server.ino
 Created:	7/17/2019 8:46:06 PM
 Author:	kevin

 Version History:
 V1.0.1			27 Dec 2022 -			:
 V1.0.0			<	27 Dec 2022			:		all basic and major functionality, including web services and UDP discovery service as
												well as a pixel rendering engine



 Buffer Allocations (*** BUFFER ALLOCATION ***)
	--- WEB BUFFERS ---
	4000: *** BUFFER ALLOCATION *** - Web receiving buffer
	200:  *** BUFFER ALLOCATION *** - Web response buffer
	200:  *** BUFFER ALLOCATION *** - Web response JSON document buffer
	100:  *** BUFFER ALLOCATION *** - Web response info buffer
	----
	4500

	--- INDIVIDUAL LPIs ---
	500:  *** BUFFER ALLOCATION *** - Individual LPI loading buffer
	500:  *** BUFFER ALLOCATION *** - Individual LPI for validation
	500:  *** BUFFER ALLOCATION *** - Individual LPI building
	----
	1500

	--- ENTIRE LPs ---
	4000: *** BUFFER ALLOCATION *** - An entire LP program requiring validation
	4000: *** BUFFER ALLOCATION *** - Store an entire LP state
	xxxx: *** BUFFER ALLOCATION *** - Store a tree structure of a entire LP state
	----
	8000+

 TOTAL: 14000


 IMPORTANT INFORMATION:-

 1. Debugging via serial
    - If wanting to debug, do the following:
	  1. Edit Credentials.h and set:
	      * Line 42: set TO_LOAD_DEFAULT_CONFIG_DATA to true
		  * Line 55: set LOAD_DEFAULT_CONFIG_DATA to true
		  * Line 64: set your wifi credentials
		  This bypasses the wifi portal / setup and loads specified Wifi credentials.  NOTE: with serial
		  output ON, the wifi setup DOES NOT work.
	  2. Edit AppLogger.h and set:
	      * Line 22: comment out this line
		  This enables serial output.
	  3. Optional: Edit LightServerOrchastrator.h and set:
	      * Line 16: ensure not commmented out
		  This outputs server life-cycle messages.  NOTE: there are a lot of data output.

	  NOTE: after uploading the build, wifi will only connect once you connect to the serial port
	  and start receiving messages from the board.
2.  Run server in production mode:
     - To run the server as if in production mode:
	  1. Do the opposite of debugging via serial.

UPDATE 17/08/2021: Simply define DEBUG_MODE to enable serial output or commect out for production mode
*/


// Define DEBUG_MODE for serial output, comment out for production mode
#define		DEBUG_MODE						true

#define		MIN_LEDS						10
#define		MAX_LEDS						350

// 3500 = CRASH @ 233 LEDS
#define		BUFFER_SIZE						3500		// 4000
#define		BUFFER_JSON_RESPONSE_SIZE		150			// 200
#define		BUFFER_WEB_RESPONSE_SIZE		150			// 200

#define		PIN								6			// port # of LED controller
// #define		NUMLEDS							50			// default number of connected LEDs if no configurtion values
#define		NUMLEDS							350			// default number of connected LEDs if no configurtion values
#define		RENDERING_FRAME					25			// rendering frame duration in milliseconds

#define		LS_VERSION						"1.0.1"		// Light-server version
#define		LDL_VERSION						"1.0.0"		// Light-definition language version

/* Networkng constants */
#define		BASIC_AUTH_SUPER				"U3VwZXI6MXhZYTFtYW4yKg=="	// Super:1xYa1man2*

#define		DISCOVERY_HANDSHAKE_MSG			"LDL-HOLA?"
#define		DISCOVERY_PORT					8888
// The following is the response that is sent to a UDP discovery request.  The "name"
// value is the device name.  It is intented that this be a user-friendly set of 2-3 words proceeded by LDL-
// e.g. LDL-BlueMelon, LDL-RedApple, LDL-GreenPear.  An enhancement here would be to allow
// the user to overwrite the default friendly name although I'm sure many users would not bother.
const char DISCOVERY_FOUND_MSG[] PROGMEM = "{ \"server\" : \"1.0.1\", \"name\" : \"LDL-Window\" }";
char discoveryResponse[BUFFER_JSON_RESPONSE_SIZE];
// #define		WEBDUINO_SERIAL_DEBUGGING	2		// define this to see web server debugging output

// MKR-Wifi
#define		MKR1010


#include<avr/pgmspace.h>

// Networking
#include "src/Networking/WifiConnectManager/defines.h"
#include "src/Networking/WifiConnectManager/Credentials.h"
#include "src/Networking/WifiConnectManager/dynamicParams.h"

// 1. Timer
#include "src/Orchastrator/ArduinoTimer.h"
// 2. LpExecutor
#include "src/LPE/LpiExecutors/LpiExecutorFactory.h"
#include "src/LPE/Executor/LpExecutor.h"
// 3. LpState
#include "src/LPE/StateBuilder/LpJsonState.h"
// 4. PixelRenderer
#include "src/Adafruit_NeoPixel.h"
#include "src/Adafruit_NeoPixel.h"
#include "src/Renderer/PixelRenderer.h"
// 5. LightWebServer
#include "src/FixedSizeCharBuffer.h"
#include "src/LightWebServer.h"
#include "src/WebServer.h"
// 6. CommandFactory
#include "src/LPE/Validation/JsonInstructionValidatorFactory.h"
#include "src/LPE/Validation/LpJsonValidator.h"
#include "src/LPE/StateBuilder/JsonInstructionBuilderFactory.h"
#include "src/LPE/StateBuilder/LpJsonStateBuilder.h"
#include "src/Commands/CommandFactory.h"
#include "src/AppLogger.h"
#include "src/Orchastrator/LightServerOrchastrator.h"
// 7. Individual commands
#include "src/Commands/NoAuthCommand.h"
#include "src/Commands/InvalidCommand.h"
#include "src/Commands/LoadProgramCommand.h"
#include "src/Commands/LoadProgramAndStoreCommand.h"
#include "src/Commands/PowerOffCommand.h"
#include "src/Commands/PowerOnCommand.h"
#include "src/Commands/CheckPowerCommand.h"
#include "src/Commands/GetAboutCommand.h"
#include "src/ConfigPersistance/IConfigPersistance.h"
#include "src/ConfigPersistance/FlashConfigPersistance.h"
#include "src/Commands/SetLedsCommand.h"

// 8. Networking
#include "src/Networking/EthernetUdpService.h"
#include "src/Networking/EthernetUdpDiscoveryService.h"

// Utiltiy functions
#include "UtilityFunctions.h"

WiFiManager_NINA_Lite* WiFiManager_NINA;
WiFiNINA_Configuration myConfig;

// Instantiate dependencies required by LightServerOrchastrator
// 1. Timer: for determining when the next instruction should be rendered
LS::ArduinoTimer timer(RENDERING_FRAME);
// 2. LpExecutor: executes Light Program to determine next rendering instruction
LS::LpiExecutorFactory lpiExecutorFactory = LS::LpiExecutorFactory();
LS::StringProcessor stringProcessor;
LS::FlashConfigPersistance configPersistance = LS::FlashConfigPersistance();
LS::LEDConfig ledConfig = LS::LEDConfig();
LS::LpExecutor executor = LS::LpExecutor(&lpiExecutorFactory, &stringProcessor, &ledConfig);
// 3. LpState: stores the tree representation of a parsed Light Program
LS::LpJsonState primaryState;
// 4. PixelRenderer: interacts with and activates individual LEDs on the connected hardware
Adafruit_NeoPixel pixels(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);
LS::PixelRenderer renderer = LS::PixelRenderer(&pixels, &ledConfig);
// 5. ILightServer: receives and executes HTTP commands
WebServer webserv("", 80);
LS::IWebServer* webserver = &webserv;
// *** BUFFER ALLOCATION *** - Web receiving buffer
LS::FixedSizeCharBuffer webLoadingBuffer = LS::FixedSizeCharBuffer(BUFFER_SIZE);
LS::LightWebServer lightWebServ(webserver, &webLoadingBuffer, BASIC_AUTH_SUPER);
// 6. CommandFactory: returns command instances for received HTTP commands.  These are then executed.
LS::CommandFactory commandFactory;
// ** Orchastrator **
LS::LightServerOrchastrator orchastrator = LS::LightServerOrchastrator(
	&timer,
	&executor,
	&primaryState,
	&renderer,
	&lightWebServ,
	&commandFactory
);
// 7. Individual commands that are added to the command factory
LS::NoAuthCommand noAuthCommand = LS::NoAuthCommand(&lightWebServ);
LS::InvalidCommand invalidCommand = LS::InvalidCommand(&lightWebServ);
LS::JsonInstructionValidatorFactory instructionValidatorFactory = LS::JsonInstructionValidatorFactory(&lpiExecutorFactory, &stringProcessor, &ledConfig);
LS::LpJsonValidator validator = LS::LpJsonValidator(&instructionValidatorFactory);
LS::JsonInstructionBuilderFactory instructionBuilderFactory = LS::JsonInstructionBuilderFactory(&lpiExecutorFactory, &stringProcessor, &ledConfig);
LS::LpJsonStateBuilder stateBuilder = LS::LpJsonStateBuilder(&instructionBuilderFactory);
LS::LoadProgramCommand loadProgramCommand = LS::LoadProgramCommand(&lightWebServ, &validator, &stateBuilder, &primaryState);
LS::LoadProgramAndStoreCommand loadProgramAndStoreCommand = LS::LoadProgramAndStoreCommand(&lightWebServ, &validator, &stateBuilder, &primaryState, &ledConfig, &configPersistance);
LS::PowerOffCommand powerOffCommand = LS::PowerOffCommand(&lightWebServ, &pixels, &orchastrator);
LS::PowerOnCommand powerOnCommand = LS::PowerOnCommand(&lightWebServ, &pixels, &orchastrator, &stringProcessor);
// *** BUFFER ALLOCATION *** - Web response JSON document buffer
StaticJsonDocument<BUFFER_JSON_RESPONSE_SIZE> webDoc;
// ***BUFFER ALLOCATION*** - Web response buffer
LS::FixedSizeCharBuffer webReponse = LS::FixedSizeCharBuffer(BUFFER_WEB_RESPONSE_SIZE);
LS::CheckPowerCommand checkPowerCommand = LS::CheckPowerCommand(&lightWebServ, &pixels, &webDoc, &webReponse);
LS::GetAboutCommand getAboutCommand = LS::GetAboutCommand(&lightWebServ, &webDoc, &webReponse, &ledConfig);
LS::SetLedsCommand setLedsCommand = LS::SetLedsCommand(&lightWebServ, &stringProcessor, &ledConfig, &configPersistance, &pixels, &primaryState);

LS::AppLogger appLogger;
// 8: Networking: e.g. UDP discovery service
LS::EthernetUdpService ethernetUdpService;
LS::EthernetUdpDiscoveryService discoveryService = LS::EthernetUdpDiscoveryService(DISCOVERY_PORT, DISCOVERY_FOUND_MSG, DISCOVERY_HANDSHAKE_MSG, &ethernetUdpService);

// WiFiManager_NINA_Lite* WiFiManager_NINA;


#if USING_CUSTOMS_STYLE
const char NewCustomsStyle[] /* PROGMEM */ = "<style>div,input{padding:5px;font-size:1em;}input{width:95%;}body{text-align: center;}\
button{background-color:blue;color:white;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.3rem;margin:0px;}</style>";
#endif

// char udpReply[200];

// Complex XMAS program - this is the default program is no other program has been permanently stored to flash memory
const char defaultLdlProgram[] PROGMEM = "{\"name\":\"Complexxmastree\",\"instructions\":[{\"repeat\":{\"times\":0,\"instructions\":[{\"repeat\":{\"times\":2,\"instructions\":[\"07010000193C002FF000033CC00\"]}},{\"repeat\":{\"times\":4,\"instructions\":[\"040100000A0000000FF0000\",\"040100000A1FF0000000000\",\"040100000A000000033CC00\",\"040100000A133CC00000000\"]}},{\"repeat\":{\"times\":6,\"instructions\":[\"030100000100F0F33CC00FF0000\",\"030100000110F0F33CC00FF0000\"]}},{\"repeat\":{\"times\":30,\"instructions\":[\"0528000002FF000033CC00\"]}},{\"repeat\":{\"times\":8,\"instructions\":[\"07010000193C003FF000033CC003366FF\"]}},{\"repeat\":{\"times\":10,\"instructions\":[\"030100000100F0FFF000033CC00\",\"030100000110F0FFF000033CC00\"]}},{\"repeat\":{\"times\":10,\"instructions\":[\"030100000100F0FFFFFFF000000\",\"030100000110F0FFFFFFF000000\"]}}]}}]}";



void setup() {
	// ledConfig.numberOfLEDs = NUMLEDS;

	initOnboardRgbLed();
	// yellow = booting up
	setOnboardRgbLed(255, 255, 0);

	LS::IAppLogger* appLog = (LS::IAppLogger*)&appLogger;
	appLogger.StartLogging();
	WiFiManager_NINA = new WiFiManager_NINA_Lite();

	// Optional to change default AP IP(192.168.4.1) and channel(10)
	//WiFiManager_NINA->setConfigPortalIP(IPAddress(192, 168, 120, 1));
	WiFiManager_NINA->setConfigPortalChannel(0);
#if USING_CUSTOMS_STYLE
	WiFiManager_NINA->setCustomsStyle(NewCustomsStyle);
#endif
#if USING_CUSTOMS_HEAD_ELEMENT
	WiFiManager_NINA->setCustomsHeadElement("<style>html{filter: invert(10%);}</style>");
#endif
#if USING_CORS_FEATURE  
	WiFiManager_NINA->setCORSHeader("Your Access-Control-Allow-Origin");
#endif
	// Set customized DHCP HostName
	WiFiManager_NINA->begin(HOST_NAME);
	//Or use default Hostname "SAMD-WiFiNINA-XXXXXX"
	//WiFiManager_NINA->begin();

	// add the individual command references to the command factory
	commandFactory.SetCommand(LS::CommandType::NOAUTH, &noAuthCommand);
	commandFactory.SetCommand(LS::CommandType::INVALID, &invalidCommand);
	commandFactory.SetCommand(LS::CommandType::LOADPROGRAM, &loadProgramCommand);
	commandFactory.SetCommand(LS::CommandType::LOADPROGRAMANDSTORE, &loadProgramAndStoreCommand);
	commandFactory.SetCommand(LS::CommandType::POWEROFF, &powerOffCommand);
	commandFactory.SetCommand(LS::CommandType::POWERON, &powerOnCommand);
	commandFactory.SetCommand(LS::CommandType::CHECKPOWER, &checkPowerCommand);
	commandFactory.SetCommand(LS::CommandType::GETABOUT, &getAboutCommand);
	commandFactory.SetCommand(LS::CommandType::SETLEDS, &setLedsCommand);


	// add the app logger class so the orchastrator can log events for debugging purposes
	orchastrator.SetAppLogger(appLog);

	// start the pixel renderer
	pixels.begin();

	// start the web server listening for requests
	lightWebServ.Start();
	
	// ensure the orchastrator, ready to execute on the next rendering frame cycle
	orchastrator.Start();

	// start listening for incoming UDP packets so the IP address of this server can be discovered
	discoveryService.StartDiscoveryService();

	// attempt to read the LED configuration from flash - use defaults if no config values or invalid
	ledConfig = configPersistance.ReadConfig();
	bool updateLedLength = false;
	if (ledConfig.numberOfLEDs == 0) {
		// no # of LEDs saved to config, so this is probably the first time
		// this has been run.  Therefore, check if a valid value for number
		// of LEDs was supplied along with the connection params.
		int initialNumberOfLeds = atoi(myMenuItems[0].pdata);
		if (initialNumberOfLeds >= MIN_LEDS && initialNumberOfLeds <= MAX_LEDS) {
			// yes, a valid value was supplied so we can now persist that value
			// to config for future resets
			ledConfig.numberOfLEDs = initialNumberOfLeds;
			configPersistance.SaveConfig(&ledConfig);
			updateLedLength = true;
		}
		else {
			// not initialised so set defaults
			// ledConfig.numberOfLEDs = NUM_LEDS;
			ledConfig.numberOfLEDs = MIN_LEDS;
		}
	}
	else {
		updateLedLength = true;
	}

	if (updateLedLength) {
		pixels.fill(0, 0, 0);
		pixels.show();
		pixels.updateLength(ledConfig.numberOfLEDs);
	}

	// Re-load the last loaded program or otherwise use a default program if nothing has yet been persisted to flash
	const char* defaultProgram = defaultLdlProgram;
	if (ledConfig.storedProgram != nullptr && strlen(ledConfig.storedProgram) > 0) {
		defaultProgram = ledConfig.storedProgram;
	}
	webLoadingBuffer.LoadFromBuffer(defaultProgram);
	stateBuilder.BuildState(&webLoadingBuffer, &primaryState);

	// Set the name of the server to be the name supplied during configuration
	// This is in response to a GET \about request
	WiFiManager_NINA->getFullConfigData(&myConfig);
	getAboutCommand.setServerName(myConfig.board_name);

	// Also, set the name of the server when responding to a UDP discovery request
	webDoc.clear();
	(webDoc)["server"] = LS_VERSION;
	(webDoc)["name"] = myConfig.board_name;
	serializeJsonPretty(webDoc, discoveryResponse, BUFFER_JSON_RESPONSE_SIZE);
	discoveryService.SetReplyMessage(discoveryResponse);
}

void loop() {
	// display wifi connect web page if no wifi credentials yet supplied, otherwise
	// checks wifi is connected and re-connects if necessary
	WiFiManager_NINA->run();
	
	// output an indication of the wifi status on the RGB LED
	checkWifistatus(WiFiManager_NINA);
	bool wifiIsConnected = WiFi.status() == WL_CONNECTED;

	// check if a UDP packet has been received
	discoveryService.CheckForHandshake();
	// NOTE:
	// if UDP discovery is not working then check if there is a 2nd ethernet adapter.In particular, if
	// a VirtualBox adapter is enabled it will prevent the UDP service from functioning.In this case
	// disable the VirtualBox adapter.

	// execute the orchastrator on each loop - this will just return if not yet time to execute
	orchastrator.Execute(!wifiIsConnected);
}