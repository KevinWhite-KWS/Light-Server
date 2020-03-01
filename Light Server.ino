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

#define		WEBDUINO_SERIAL_DEBUGGING	2		// define this to see web server debugging output

#include <SPI.h>
#include <Ethernet.h>

#include "src/Renderer.h"
#include "src/Adafruit_NeoPixel.h"
#include "src/FixedSizeCharBuffer.h"
#include "src/LPE.h"
#include "src/LightWebServer.h"
#include "src/CommandExecutor.h"
#include "src/AppLogger.h"
#include "src/WebServer.h"


static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static uint8_t ip[] = { 192, 168, 1, 210 };

// Instantiate the classes that handle activating the LEDS
LS::LEDConfig ledConfig = LS::LEDConfig(NUMLEDS);
Adafruit_NeoPixel pixels(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);

// Instantiate the classes that handle the web interface
WebServer webserv("", 80);
LS::IWebServer* webserver = &webserv;
LS::FixedSizeCharBuffer webLoadingBuffer = LS::FixedSizeCharBuffer(BUFFER_SIZE);
LS::LightWebServer lightWebServ(webserver, &webLoadingBuffer, BASIC_AUTH_SUPER);

LS::CommandExecutor* commandExecutor = nullptr;
void setup() {
	LS::IAppLogger* appLogger = new LS::AppLogger();
	commandExecutor = new LS::CommandExecutor(&pixels, &ledConfig, &lightWebServ, appLogger);

	pixels.begin();

	// Disable the SD card reader
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);

	// initialize the Ethernet adapter
	Ethernet.begin(mac, ip);
	lightWebServ.Start();

	commandExecutor->Start();
}


void loop() {
	commandExecutor->Execute();
}

