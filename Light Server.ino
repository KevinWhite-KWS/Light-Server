/*
 Name:		Light_Server.ino
 Created:	7/17/2019 8:46:06 PM
 Author:	kevin
*/


/*
	IDEAS:
		* 23/09/2019: Web to translate a few words into morse code (i.e. write the light program to represent the morse code and send it to the LS).
					  Text to Morse Code.
		* 23/09/2019: Display an event on the LS e.g. email received so run an email notification program.
		* 23/09/2019: Allow temperature to be displayed on LEDs by getting latest temperature and writing a program for it to display that.


	TODO:
		* 05/12/2019: Re-factor LPE as some of the code is a little messy now
		* 05/11/2019: Possibly consider implement the ability to run an interrupt program?  (Due allows this easily due to resources)
		* 05/11/2019: Add-in request authentication

	DONE:
		* 08/08/2019: Add a test class for the ClearNonAnimatedLPI class (methods: Validate, GetTotalNumberOfSteps, GetNextRI)
		* 08/08/2019: Implement ClearNonAnimatedLPI class
		* 08/08/2019: Add a method to StringProcessor to extract an instruction into a structure (ins, duration, x, y)
		* 20/08/2019: Add a test method for the new method to extract an instruction into a structure
		* 01/08/2019: Add the AreAnyLEDsOn():bool method + unit tests (should only be true if any single LED is not black) [DONE - 08/08/2019]
		* 01/08/2019: Set up a test Arduino program to check the timings i.e. how quickly does SetRI execute given different inputs?
					  How quickly does Render execute?  Are any issues forseen with the strict timing requirements?
					  [DONE - 08/08/2019]
					  [SetRI takes approx. 6-7 ms for 200 LEDs using a complex instruction]
					  [Render takes approx. 1 ms for 200 LEDs]
		* 28/07/2019: Complete the SetRI method: set pixels as an RI instruction is extracted until all LEDs are filled or end of RI. [DONE - 01/08/2019]
		* 29/07/2019: Add an Adafruit point to the Renderer constructor [DONE - 29/07/2019]
		* 29/07/2019: Find a way, if possible, to mock in C++ complex classes like Adafruit_Neopixel. [DONE - 29/07/2019]
		* 28/07/2019: Add LEDConfig structure type. [DONE - 29/07/2019]
		* 28/07/2019: Modify Renderer constructor to include LEDConfig pointer. [DONE - 29/07/2019]
		* 28/07/2019: Add SetLEDConfig method to Renderer (build relevent double buffer size, etc.). [DONE - 29/07/2019]
		* 

	MKR 1010 changes / problems
		(MKR-Flash)		Flash config persistance does not work
		(MKR-Wifi)		Wifi instead of Ethernet
		(MKR-WifiUDP)	UDP using wifi instead of Ethernet UDP


	Buffer Allocations (*** BUFFER ALLOCATION ***)
		// --- WEB BUFFERS ---
		// 4000: *** BUFFER ALLOCATION *** - Web receiving buffer
		// 200:  *** BUFFER ALLOCATION *** - Web response buffer
		// 200:  *** BUFFER ALLOCATION *** - Web response JSON document buffer
		// 100:  *** BUFFER ALLOCATION *** - Web response info buffer
		   ----
		   4500

		// --- INDIVIDUAL LPIs ---
		// 500:  *** BUFFER ALLOCATION *** - Individual LPI loading buffer
		// 500:  *** BUFFER ALLOCATION *** - Individual LPI for validation
		// 500:  *** BUFFER ALLOCATION *** - Individual LPI building
		   ----
		   1500

		// --- ENTIRE LPs ---
		// 4000: *** BUFFER ALLOCATION *** - An entire LP program requiring validation
		// 4000: *** BUFFER ALLOCATION *** - Store an entire LP state
		// xxxx: *** BUFFER ALLOCATION *** - Store a tree structure of a entire LP state
		   ----
		   8000+


		TOTAL: 14000
*/

//#ifdef __arm__
//#define BUFFER_SIZE		10000	// Due
//#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
//#define BUFFER_SIZE		1000	// Mega
//#endif  // __arm__
#define		BUFFER_SIZE						4000 // 1500
#define		BUFFER_JSON_RESPONSE_SIZE		200
#define		BUFFER_WEB_RESPONSE_SIZE		200


#define		PIN					6
#define		NUMLEDS				50

// individual pixels: 8
// small strip has: 18
// long strip has: 118
#define		RENDERING_FRAME		25

#define		LS_VERSION			"1.0.0"
#define		LDL_VERSION			"1.0.0"

#define		BASIC_AUTH_SUPER	"U3VwZXI6MXhZYTFtYW4yKg=="	// Super:1xYa1man2*

#define		DISCOVERY_HANDSHAKE_MSG		"LDL-HOLA?"
#define		DISCOVERY_PORT				8888
#define		DISCOVERY_FOUND_MSG			"{ \"server\" : \"1.0.0\" }"

// #define		WEBDUINO_SERIAL_DEBUGGING	2		// define this to see web server debugging output


namespace LS {
	// #define		ORCHASTRATOR_DEBUG_ALL		// define this to see orchastrator debugging output
}


// MKR-Wifi
// #include <Ethernet.h>
// #include <EthernetUdp.h>
#define		MKR1010






/*** START: WIFI Manager ***/
////char ssid[] = "LittleDaddy";   // your network SSID (name)
////char pass[] = "rodent467*";    // your network password (use for WPA, or use as key for WEP)
////#include <SPI.h>
////#include <WiFiNINA.h>
////#include <utility/wifi_drv.h>
#include "src/Networking/WifiConnectManager/defines.h"
#include "src/Networking/WifiConnectManager/Credentials.h"
#include "src/Networking/WifiConnectManager/dynamicParams.h"
/*** END: WIFI Manager ***/



// 1. Timer
#include "src/Orchastrator/ArduinoTimer.h"
// 2. LpExecutor
#include "src/LPE/LpiExecutors/LpiExecutorFactory.h"
#include "src/LPE/Executor/LpExecutor.h"
// 3. LpState
#include "src/LPE/StateBuilder/LpJsonState.h"
// 4. PixelRenderer
#include "src/Adafruit_NeoPixel.h"
#include "src/Renderer/PixelRenderer.h"
// 5. LightWebServer
#include "src/FixedSizeCharBuffer.h"
#include "src/LightWebServer.h"
// TEMP
#include "src/WebServer.h"
// 6. CommandFactory
#include "src/LPE/Validation/JsonInstructionValidatorFactory.h"
#include "src/LPE/Validation/LpJsonValidator.h"
#include "src/LPE/StateBuilder/JsonInstructionBuilderFactory.h"
#include "src/LPE/StateBuilder/LpJsonStateBuilder.h"
#include "src/Commands/CommandFactory.h"
// ** Orchastrator **
#include "src/AppLogger.h"
#include "src/Orchastrator/LightServerOrchastrator.h"
// 7. Individual commands
#include "src/Commands/NoAuthCommand.h"
#include "src/Commands/InvalidCommand.h"
#include "src/Commands/LoadProgramCommand.h"
#include "src/Commands/PowerOffCommand.h"
#include "src/Commands/PowerOnCommand.h"
#include "src/Commands/CheckPowerCommand.h"
#include "src/Commands/GetAboutCommand.h"

// MKR-Flash
#include "src/ConfigPersistance/IConfigPersistance.h"
#include "src/ConfigPersistance/FlashConfigPersistance.h"
#include "src/Commands/SetLedsCommand.h"


// 8. Networking
#include "src/Networking/EthernetUdpService.h"
#include "src/Networking/EthernetUdpDiscoveryService.h"


////static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
////
////// NOTE: for when connected to the Eero
////static uint8_t ip[] = { 192, 168, 5, 210 };
////// NOTE: for when connected directly to the router
////// static uint8_t ip[] = { 192, 168, 1, 210 };
////
// Instantiate dependencies required by LightServerOrchastrator
// 1. Timer: for determining when the next instruction should be rendered
LS::ArduinoTimer timer(RENDERING_FRAME);
// 2. LpExecutor: executes Light Program to determine next rendering instruction
LS::LpiExecutorFactory lpiExecutorFactory = LS::LpiExecutorFactory();
LS::StringProcessor stringProcessor;

// MKR-Flash
LS::FlashConfigPersistance configPersistance = LS::FlashConfigPersistance();


// LS::LEDConfig ledConfig = LS::LEDConfig(NUMLEDS);
LS::LEDConfig ledConfig = LS::LEDConfig();
LS::LpExecutor executor = LS::LpExecutor(&lpiExecutorFactory, &stringProcessor, &ledConfig);
// 3. LpState: stores the tree representation of a parsed Light Program
LS::LpJsonState primaryState;
// 4. PixelRenderer: interacts with and activates individual LEDs on the connected hardware
Adafruit_NeoPixel pixels(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);
LS::PixelRenderer renderer = LS::PixelRenderer(&pixels, &ledConfig);
// 5. ILightServer: receives and executes HTTP commands
// TEMP
WebServer webserv("", 80);
LS::IWebServer* webserver = &webserv;
// LS::IWebServer* webserver = nullptr;
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
LS::PowerOffCommand powerOffCommand = LS::PowerOffCommand(&lightWebServ, &pixels, &orchastrator);
LS::PowerOnCommand powerOnCommand = LS::PowerOnCommand(&lightWebServ, &pixels, &orchastrator, &stringProcessor);
// *** BUFFER ALLOCATION *** - Web response JSON document buffer
StaticJsonDocument<BUFFER_JSON_RESPONSE_SIZE> webDoc;
// ***BUFFER ALLOCATION*** - Web response buffer
LS::FixedSizeCharBuffer webReponse = LS::FixedSizeCharBuffer(BUFFER_WEB_RESPONSE_SIZE);
LS::CheckPowerCommand checkPowerCommand = LS::CheckPowerCommand(&lightWebServ, &pixels, &webDoc, &webReponse);
LS::GetAboutCommand getAboutCommand = LS::GetAboutCommand(&lightWebServ, &webDoc, &webReponse, &ledConfig);

// MKR-Flash
LS::SetLedsCommand setLedsCommand = LS::SetLedsCommand(&lightWebServ, &stringProcessor, &ledConfig, &configPersistance, &pixels, &primaryState);
// LS::SetLedsCommand setLedsCommand = LS::SetLedsCommand(&lightWebServ, &stringProcessor, &ledConfig, nullptr, &pixels);
LS::AppLogger appLogger;
// 8: Networking: e.g. UDP discovery service
LS::EthernetUdpService ethernetUdpService;
LS::EthernetUdpDiscoveryService discoveryService = LS::EthernetUdpDiscoveryService(DISCOVERY_PORT, DISCOVERY_FOUND_MSG, DISCOVERY_HANDSHAKE_MSG, &ethernetUdpService);

/*** START: WIFI Manager ***/
//// int status = WL_IDLE_STATUS;
void heartBeatPrint(void)
{
	static int num = 1;

	if (WiFi.status() == WL_CONNECTED) {
		// Serial.print("H");        // H means connected to WiFi
		// green = connected to Wifi
		setOnboardRgbLed(0, 255, 0);

		// Serial.println(freeMemory());

	}
	else {
		// Serial.print("F");        // F means not connected to WiFi
		// red = not connected to Wifi
		setOnboardRgbLed(255, 0, 0);

		// Serial.println(freeMemory());
	}

	if (num == 80)
	{
		// Serial.println();
		num = 1;
	}
	else if (num++ % 10 == 0)
	{
		// Serial.print(" ");
	}
}

void check_status()
{
	static unsigned long checkstatus_timeout = 0;

	//KH
#define HEARTBEAT_INTERVAL    20000L
  // Print hearbeat every HEARTBEAT_INTERVAL (20) seconds.
	if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
	{
		heartBeatPrint();
		checkstatus_timeout = millis() + HEARTBEAT_INTERVAL;
	}
}

WiFiManager_NINA_Lite* WiFiManager_NINA;

#if USING_CUSTOMS_STYLE
const char NewCustomsStyle[] /*PROGMEM*/ = "<style>div,input{padding:5px;font-size:1em;}input{width:95%;}body{text-align: center;}\
button{background-color:blue;color:white;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.3rem;margin:0px;}</style>";
#endif
/*** END: WIFI Manager ***/




void setup() {
	ledConfig.numberOfLEDs = NUMLEDS;

	initOnboardRgbLed();
	// amber = not yet connected to wifi or initialised
	setOnboardRgbLed(255, 191, 0);


	LS::IAppLogger* appLog = (LS::IAppLogger*)&appLogger;
	appLogger.StartLogging();

	
	
	/*** START: WIFI Manager ***/
	//Serial.print(F("\nStarting SAMD_WiFiNINA on ")); Serial.println(BOARD_TYPE);
	// Serial.println(WIFIMANAGER_NINA_LITE_VERSION);

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

	/*** END: WIFI Manager ***/



	/*** START: WIFI Manager ***/
	// red flashing = connecting to Wifi
	////bool isRedHigh = 1;
	////while (status != WL_CONNECTED) {
	////	if (isRedHigh) {
	////		setOnboardRgbLed(255, 0, 0);
	////	}
	////	else {
	////		setOnboardRgbLed(0, 0, 0);
	////	}
	////	isRedHigh = !isRedHigh;


	////	// Serial.print("Attempting to connect to SSID: ");
	////	// Serial.println(ssid);
	////	// Serial.print("Attempting to connect to SSID: ");
	////	// Serial.println(ssid);
	////	//serialprint("Attempting to connect to SSID: ");
	////	//serialprintln(ssid);
	////	appLogger.logEvent(0, 0, "Attempting to connect to SSID: ", "STARTUP");
	////	appLogger.logEvent(0, 0, ssid, "STARTUP");
	////	
	////	// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
	////	status = WiFi.begin(ssid, pass);
	////	// setOnboardRgbLed(0, 0, 255);

	////	// wait 1 second for connection:
	////	delay(1000);
	////}
	////// amber = wifi connected
	////setOnboardRgbLed(255, 191, 0);
	////printWifiStatus();
	/*** END: WIFI Manager ***/

	// add the individual command references to the command factory
	commandFactory.SetCommand(LS::CommandType::NOAUTH, &noAuthCommand);
	commandFactory.SetCommand(LS::CommandType::INVALID, &invalidCommand);
	commandFactory.SetCommand(LS::CommandType::LOADPROGRAM, &loadProgramCommand);
	commandFactory.SetCommand(LS::CommandType::POWEROFF, &powerOffCommand);
	commandFactory.SetCommand(LS::CommandType::POWERON, &powerOnCommand);
	commandFactory.SetCommand(LS::CommandType::CHECKPOWER, &checkPowerCommand);
	commandFactory.SetCommand(LS::CommandType::GETABOUT, &getAboutCommand);
	commandFactory.SetCommand(LS::CommandType::SETLEDS, &setLedsCommand);

	// add the app logger class so the orchastrator can log events
	// for debugging purposes
	

	orchastrator.SetAppLogger(appLog);

	// start the pixel renderer
	pixels.begin();

	// MKR-Wifi
	// Disable the SD card reader
	// pinMode(4, OUTPUT);
	// digitalWrite(4, HIGH);

	// MKR-Wifi
	// initialize the Ethernet adapter

	//while (Ethernet.begin(mac) == 0) {
	//	Serial.println("Failed to configure Ethernet using DHCP");
	//	if (Ethernet.hardwareStatus() == EthernetNoHardware) {
	//		Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
	//	}
	//	else if (Ethernet.linkStatus() == LinkOFF) {
	//		Serial.println("Ethernet cable is not connected.");
	//	}

	//	delay(1);
	//}
	//Serial.print("My IP address: ");
	//Serial.println(Ethernet.localIP());

	// Ethernet.begin(mac, ip);
	lightWebServ.Start();

	// ensure the orchastrator is ready to execyte
	orchastrator.Start();

	// *** DISCOVERY SERVICE ***
	// start listening for incoming UDP packets
	// udp.begin(localPort);
	discoveryService.StartDiscoveryService();

	// attempt to read the LED configuration from flash - use defaults if no config values or invalid
	// MKR-Flash
	// LS::LEDConfig newConfig;
	ledConfig = configPersistance.ReadConfig();
	if (ledConfig.numberOfLEDs == 0) {
		// not initialised so set defaults
		ledConfig.numberOfLEDs = NUMLEDS;
	}
	else {
		pixels.fill(0, 0, 0);
		pixels.show();
		pixels.updateLength(ledConfig.numberOfLEDs);
	}


	// configPersistance.SaveConfig(&newConfig);
	
	/*
	Serial.println("Reading config from Flash");
	bool configIsValid = configPersistance.ReadConfig(&ledConfig);
	if (configIsValid == false) {
		Serial.println("Config not valid - using defaults");
		// default configuration values
		ledConfig.numberOfLEDs = NUMLEDS;
		ledConfig.controlValue = 99;
	}
	else {
		Serial.print("No LEDS: ");
		Serial.println(ledConfig.numberOfLEDs);
		pixels.fill(0, 0, 0);
		pixels.show();
		pixels.updateLength(ledConfig.numberOfLEDs);
	}
	*/



	// green = fully initialised and ready to go
	// setOnboardRgbLed(0, 255, 0);
}


void loop() {
	// TODO: every x seconds we need to check if Wifi still connected
	// if not connected we need to re-connect it

	/*** START: WIFI Manager ***/
	WiFiManager_NINA->run();
	check_status();
	/*** END: WIFI Manager ***/

	// *** DISCOVERY SERVICE ***
	// check if a UDP packet has been received

	discoveryService.CheckForHandshake();
	
	
	// NOTE:
	// if UDP discovery is not working then check if there is a 2nd ethernet adapter.In particular, if
	// a VirtualBox adapter is enabled it will prevent the UDP service from functioning.In this case
	// disable the VirtualBox adapter.


	// execute the orchastrator on each loop.  If it's not time
	// then Execute will simply return with no action performed.
	
	orchastrator.Execute();
}

/*** START: WIFI Manager ***/
////void printWifiStatus() {
////	// print the SSID of the network you're attached to:
////	appLogger.logEvent(0, 0, "SSID: ", "STARTUP");
////	appLogger.logEvent(0, 0, WiFi.SSID(), "STARTUP");
////
////	IPAddress ip = WiFi.localIP();
////	appLogger.logEvent(0, 0, "IP Adddress: ", "STARTUP");
////	// appLogger.logEvent(0, 0, , "STARTUP");
////
////
////
////	/*
////	serialprint("SSID: ");
////	serialprintln(WiFi.SSID());
////
////	// print your board's IP address:
////	IPAddress ip = WiFi.localIP();
////	serialprint("IP Address: ");
////	serialprintln(ip);
////
////	// print the received signal strength:
////	long rssi = WiFi.RSSI();
////	serialprint("signal strength (RSSI):");
////	serialprint(rssi);
////	serialprintln(" dBm");
////
////	serialprintln("V1.1");
////	*/
////}
/*** END: WIFI Manager ***/

#define ONBOARD_LED_GREEN_PIN		25
#define	ONBOARD_LED_RED_PIN			26
#define ONBOARD_LED_BLUE_PIN		27
void setOnboardRgbLed(uint8_t red, uint8_t green, uint8_t blue) {
#ifdef MKR1010
	WiFiDrv::analogWrite(ONBOARD_LED_GREEN_PIN, green); //GREEN
	WiFiDrv::analogWrite(ONBOARD_LED_RED_PIN, red);   //RED
	WiFiDrv::analogWrite(ONBOARD_LED_BLUE_PIN, blue);   //BLUE
#endif
}

void initOnboardRgbLed() {
#ifdef MKR1010
	WiFiDrv::pinMode(ONBOARD_LED_GREEN_PIN, OUTPUT);
	WiFiDrv::pinMode(ONBOARD_LED_RED_PIN, OUTPUT);
	WiFiDrv::pinMode(ONBOARD_LED_BLUE_PIN, OUTPUT);
	setOnboardRgbLed(0, 0, 0);
#endif
}