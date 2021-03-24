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
*/

#ifdef __arm__
#define BUFFER_SIZE		10000	// Due
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
#define BUFFER_SIZE		1000	// Mega
#endif  // __arm__

#define		PIN		6
#define		NUMLEDS	18
// individual pixels: 8
// small strip has: 18
// long strip has: 118
#define		RENDERING_FRAME		50

#define		LS_VERSION			"1.0.0"
#define		LDL_VERSION			"1.0.0"

#define		BASIC_AUTH_SUPER	"U3VwZXI6MXhZYTFtYW4yKg=="	// Super:1xYa1man2*

#define		DISCOVERY_HANDSHAKE_MSG		"LDL-HOLA?"
#define		DISCOVERY_PORT				8888
#define		DISCOVERY_FOUND_MSG			"{ \"server\" : \"1.0.0\" }"

#define		WEBDUINO_SERIAL_DEBUGGING	2		// define this to see web server debugging output


namespace LS {
	// #define		ORCHASTRATOR_DEBUG_ALL		// define this to see orchastrator debugging output
}

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

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
#include "src/Commands/SetLedsCommand.h"
// 8. Networking
#include "src/Networking/EthernetUdpService.h"
#include "src/Networking/EthernetUdpDiscoveryService.h"

static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// NOTE: for when connected to the Eero
static uint8_t ip[] = { 192, 168, 5, 210 };
// NOTE: for when connected directly to the router
// static uint8_t ip[] = { 192, 168, 1, 210 };

// Instantiate dependencies required by LightServerOrchastrator
// 1. Timer: for determining when the next instruction should be rendered
LS::ArduinoTimer timer;
// 2. LpExecutor: executes Light Program to determine next rendering instruction
LS::LpiExecutorFactory lpiExecutorFactory = LS::LpiExecutorFactory();
LS::StringProcessor stringProcessor;
LS::LEDConfig ledConfig = LS::LEDConfig(NUMLEDS);
LS::LpExecutor executor = LS::LpExecutor(&lpiExecutorFactory, &stringProcessor, &ledConfig);
// 3. LpState: stores the tree representation of a parsed Light Program
LS::LpJsonState primaryState;
// 4. PixelRenderer: interacts with and activates individual LEDs on the connected hardware
Adafruit_NeoPixel pixels(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);
LS::PixelRenderer renderer = LS::PixelRenderer(&pixels, &ledConfig);
// 5. ILightServer: receives and executes HTTP commands
WebServer webserv("", 80);
LS::IWebServer* webserver = &webserv;
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
LS::FixedSizeCharBuffer lpBuffer = LS::FixedSizeCharBuffer(BUFFER_SIZE);
LS::JsonInstructionValidatorFactory instructionValidatorFactory = LS::JsonInstructionValidatorFactory(&lpiExecutorFactory, &stringProcessor, &ledConfig);
LS::LpJsonValidator validator = LS::LpJsonValidator(&instructionValidatorFactory);
LS::JsonInstructionBuilderFactory instructionBuilderFactory = LS::JsonInstructionBuilderFactory(&lpiExecutorFactory, &stringProcessor, &ledConfig);
LS::LpJsonStateBuilder stateBuilder = LS::LpJsonStateBuilder(&instructionBuilderFactory);
LS::LoadProgramCommand loadProgramCommand = LS::LoadProgramCommand(&lightWebServ, &lpBuffer, &validator, &stateBuilder, &primaryState);
LS::PowerOffCommand powerOffCommand = LS::PowerOffCommand(&lightWebServ, &pixels, &orchastrator);
LS::PowerOnCommand powerOnCommand = LS::PowerOnCommand(&lightWebServ, &pixels, &orchastrator, &stringProcessor);
StaticJsonDocument<1000> webDoc;
LS::FixedSizeCharBuffer webReponse = LS::FixedSizeCharBuffer(BUFFER_SIZE);
LS::CheckPowerCommand checkPowerCommand = LS::CheckPowerCommand(&lightWebServ, &pixels, &webDoc, &webReponse);
LS::GetAboutCommand getAboutCommand = LS::GetAboutCommand(&lightWebServ, &webDoc, &webReponse, &ledConfig);
LS::SetLedsCommand setLedsCommand = LS::SetLedsCommand(&lightWebServ, &stringProcessor);
LS::AppLogger appLogger;
// 8: Networking: e.g. UDP discovery service
LS::EthernetUdpService ethernetUdpService;
LS::EthernetUdpDiscoveryService discoveryService = LS::EthernetUdpDiscoveryService(DISCOVERY_PORT, DISCOVERY_FOUND_MSG, DISCOVERY_HANDSHAKE_MSG, &ethernetUdpService);

// *** DISCOVERY SERVICE ***
// EthernetUDP udp;
//unsigned int localPort = 8888;      // local port to listen on
//char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
//char replyBuffer[] = "LDL;1.0.0;";        // a string to send back


void setup() {
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
	
	LS::IAppLogger* appLog = (LS::IAppLogger*)&appLogger;
	appLogger.StartLogging();
	orchastrator.SetAppLogger(appLog);

	// start the pixel renderer
	pixels.begin();

	// Disable the SD card reader
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);

	// initialize the Ethernet adapter
	Serial.println("Initialize Ethernet with DHCP:");
	while (Ethernet.begin(mac) == 0) {
		Serial.println("Failed to configure Ethernet using DHCP");
		if (Ethernet.hardwareStatus() == EthernetNoHardware) {
			Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
		}
		else if (Ethernet.linkStatus() == LinkOFF) {
			Serial.println("Ethernet cable is not connected.");
		}

		delay(1);
	}
	Serial.print("My IP address: ");
	Serial.println(Ethernet.localIP());

	// Ethernet.begin(mac, ip);
	lightWebServ.Start();

	// ensure the orchastrator is ready to execyte
	orchastrator.Start();

	// *** DISCOVERY SERVICE ***
	// start listening for incoming UDP packets
	// udp.begin(localPort);
	discoveryService.StartDiscoveryService();
}


void loop() {
	// *** DISCOVERY SERVICE ***
	// check if a UDP packet has been received
	discoveryService.CheckForHandshake();
	//int packetSize = udp.parsePacket();
	//if (packetSize) {
	//	// read the packet into packetBufffer
	//	udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
	//	if (strcmp(DISCOVERY_MSG, packetBuffer) == 0) {
	//		// somehow has made contact so respond with
	//		// a reply that lets them know we are here and waiting!
	//		udp.beginPacket(udp.remoteIP(), udp.remotePort());
	//		udp.write(replyBuffer);
	//		udp.endPacket();

	//		for (int i = 0; i < UDP_TX_PACKET_MAX_SIZE; i++) {
	//			packetBuffer[i] = 0x00;
	//		}
	//	}
	//}

	// execute the orchastrator on each loop.  If it's not time
	// then Execute will simply return with no action performed.
	orchastrator.Execute();
}

