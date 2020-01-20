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


#include "src/Renderer.h"
#include "src/Adafruit_NeoPixel.h"
#include "src/LPI/ClearNonAnimatedLPI.h"
#include "src/LPI/SolidNonAnimatedLPI.h"
#include "src/FixedSizeCharBuffer.h"
#include "src/LPI/SliderAnimatedLPI.h"
#include "src/LPI/StochasticNonAnimatedLPI.h"
#include "src/LPE.h"
#include "src/ValueDomainTypes.h"
#include "src/MemoryFree.h"
#include "src/LightWebServer.h"

#ifdef __arm__
	#define BUFFER_SIZE		10000	// Due
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
	#define BUFFER_SIZE		1000	// Mega
#endif  // __arm__

#define		PIN		6
#define		NUMLEDS	19
// individual pixels: 8
// small strip has: 19
// long strip has: 118
#define		RENDERING_FRAME		50

#define		LS_VERSION			"1.0.0"
#define		LDL_VERSION			"1.0.0"

#define		BASIC_AUTH_SUPER	"U3VwZXI6MXhZYTFtYW4yKg=="	// Super:1xYa1man2*

using LS::StringProcessor;
using LS::Renderer;
using LS::LEDConfig;
using LS::FixedSizeCharBuffer;
using LS::ClearNonAnimatedLPI;
using LS::SolidNonAnimatedLPI;
using LS::LPIInstruction;
using LS::SliderAnimatedLPI;
using LS::StochasticNonAnimatedLPI;
using LS::LPE;
using LS::LPValidateResult;
using LS::LightWebServer;
using LS::FixedSizeCharBuffer;
using LS::CommandType;
using LS::Colour;

/* + NETWORK STUFF + */
#define WEBDUINO_SERIAL_DEBUGGING	2
#include <SPI.h>
#include <Ethernet.h>
#include "src/WebServer.h"

/* - NETWORK STUFF - */

/* + NETWORK STUFF + */
static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static uint8_t ip[] = { 192, 168, 1, 210 };
#define PREFIX ""
WebServer webserv(PREFIX, 80);
IWebServer* webserver = &webserv;
int webBufferLen = BUFFER_SIZE;
FixedSizeCharBuffer webLoadingBuffer = FixedSizeCharBuffer(BUFFER_SIZE);
LightWebServer lightWebServ(webserver, &webLoadingBuffer, BASIC_AUTH_SUPER);
/* - NETWORK STUFF - */


uint32_t nextSetRi = millis() + 39;
uint32_t currentMillis = millis();




StringProcessor stringProcessor = StringProcessor();
LEDConfig ledConfig = LEDConfig(NUMLEDS);

// Values to support rendering
Adafruit_NeoPixel pixels(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);
IPixelController* pixelController = &pixels;
Renderer render = Renderer(&stringProcessor, &ledConfig, pixelController);

// Values to support LP execution
FixedSizeCharBuffer riBuffer = FixedSizeCharBuffer(BUFFER_SIZE);
FixedSizeCharBuffer lpBuffer = FixedSizeCharBuffer(BUFFER_SIZE);
FixedSizeCharBuffer webBuffer = FixedSizeCharBuffer(BUFFER_SIZE);
FixedSizeCharBuffer webResponseBuffer = FixedSizeCharBuffer(1000);
StaticJsonDocument<1000> webResponseDoc;
LPE lpe = LPE(&ledConfig, &stringProcessor);



/*
repeat (infinite):
	ins: clear
	ins: fade in (from black to red)
	repeat (5):
		ins: slider (2 green pixels, red background, start near)
		ins: slider (2 green pixels, red background, start far)
	ins: fade  out (from red to black)
	repeat (5):
		ins: stohastic (red on black)
*/
#define LP_INFINITE_COMPLEX "{\"name\":\"complexprogram\",\"instructions\":{\"repeat\":{\"times\":0,\"instructions\":{\"instruction\":\"00190000\",\"instruction\":\"040500000A0000000FF0000\",\"repeat\":{\"times\":5,\"instructions\":{\"instruction\":\"0305000002000FF00FF0000\",\"instruction\":\"0305000002100FF00FF0000\"}},\"instruction\":\"040500000A1FF0000000000\",\"repeat\":{\"times\":5,\"instructions\":{\"instruction\":\"0532000002FF0000000000\"}}}}}}"

bool ValidateProgram() {
	// Validate the program
	unsigned long startValidate = millis();
	Serial.print("Pre-validate @ ");
	Serial.print(startValidate);
	Serial.print(".  Free memory = ");
	Serial.println(freeMemory());

	LPValidateResult result = LPValidateResult();
	lpe.ValidateLP(&lpBuffer, &result);

	Serial.print("Post-validate @ ");
	unsigned long validateDuration = millis() - startValidate;
	Serial.print(millis());
	Serial.print(".  Duration = ");
	Serial.print(validateDuration);
	Serial.print(".  Result = ");
	Serial.print(result.GetCode());
	Serial.print(".  Free memory = ");
	Serial.println(freeMemory());

	if (result.GetCode() == LS::LPValidateCode::Valid) {
		return true;
	}

	return false;
}

	// the setup function runs once when you press reset or power the board
void setup() {
	// pinMode(LED_BUILTIN, OUTPUT);
	SerialUSB.begin(115200);

	pixels.begin();

	// Set the program to be loaded here
	const char* lp = LP_INFINITE_COMPLEX;
	lpBuffer.LoadFromBuffer(lp);

	// Validate the program first
	ValidateProgram();
	
	/* + NETWORK STUFF + */
	// Disable the SD card reader
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);

	/* initialize the Ethernet adapter */
	Ethernet.begin(mac, ip);
	lightWebServ.Start();
	/* - NETWORK STUFF - */

	
}



void SingleProgramTest() {
	currentMillis = millis();

	if (currentMillis >= nextSetRi) {
		/* + NETWORK STUFF + */
		CommandType cmdType = lightWebServ.HandleNextCommand();

		if (cmdType == CommandType::INVALID) {
			lightWebServ.RespondError();
		} 
		if (cmdType == CommandType::NOAUTH) {
			lightWebServ.RespondNotAuthorised();
		}
		else if (cmdType == CommandType::POWEROFF) {
			lpe.StopLP();
			pixelController->fill(0);
			pixelController->show();
			lightWebServ.RespondNoContent();
		}
		else if (cmdType == CommandType::POWERON) {
			lpe.StopLP();

			// See if we received a valid colour
			char* buf = lightWebServ.GetLoadingBuffer(false);
			bool isValidColour = false;
			Colour colour = stringProcessor.ExtractColourFromHexEncoded(buf, isValidColour);

			if (isValidColour) {
				// Use that valid colour
				uint32_t packedColour = Adafruit_NeoPixel::Color(colour.red, colour.green, colour.blue);
				pixelController->fill(packedColour);
			}
			else {
				// Otherwise just fill them white
				pixelController->fill(16777215);
			}
			pixelController->show();
			lightWebServ.RespondNoContent();
		}
		else if (cmdType == CommandType::LOADPROGRAM) {
			char* buf = lightWebServ.GetLoadingBuffer(false);

			lpBuffer.ClearBuffer();
			lpBuffer.LoadFromBuffer(buf);
			bool valid = ValidateProgram();

			if (!valid) {
				lightWebServ.RespondError();
			}
			else {
				lightWebServ.RespondOK();
			}
		}
		else if (cmdType == CommandType::CHECKPOWER) {
			// Check whether any LED is on (i.e. a non-zero value)
			uint16_t pixelCounter = 0;
			uint16_t totalPixels = pixelController->numPixels();
			bool anyOn = false;
			while (!anyOn && pixelCounter < totalPixels) {
				anyOn = pixelController->getPixelColor(pixelCounter++) != 0;
			}

			// Respond with the body response - i.e. state of pixels
			webResponseDoc.clear();
			webResponseDoc["power"] = (anyOn ? "on" : "off");
			serializeJsonPretty(webResponseDoc, webResponseBuffer.GetBuffer(), 1000);

			lightWebServ.RespondOK(webResponseBuffer.GetBuffer());
		}
		else if (cmdType == CommandType::GETABOUT) {
			webResponseDoc.clear();
			webResponseDoc["LEDs"] = NUMLEDS;
			webResponseDoc["LS Version"] = LS_VERSION;
			webResponseDoc["LDL Version"] = LDL_VERSION;
			serializeJsonPretty(webResponseDoc, webResponseBuffer.GetBuffer(), 1000);

			lightWebServ.RespondOK(webResponseBuffer.GetBuffer());
		}
		else if (cmdType == CommandType::SETLEDS) {
			char* buf = lightWebServ.GetLoadingBuffer(false);
			bool validNumber = false;
			int newNoLeds = stringProcessor.ExtractNumberFromHexEncoded(buf, 10, 200, validNumber);
			// newNoLeds is a hex encoded value!  Dec encoded better?
			// is it consistent with the rest of the API?

			if (!validNumber) {
				lightWebServ.RespondError();
			}
			else {
				// Re-config the LEDs

				lightWebServ.RespondOK();
			}
		}

		unsigned long endWeb = millis() - currentMillis;
		//Serial.print("End check webServer @ ");
		//Serial.print(millis());
		//Serial.print(".  Duration = ");
		//Serial.print(endWeb);
		//Serial.print(".  Free memory = ");
		//Serial.println(freeMemory());
		/* - NETWORK STUFF - */




		// Output a debug message with start time of op 
		Serial.print("Start get RI @ ");
		Serial.print(currentMillis);
		Serial.print(".  Free memory = ");
		Serial.println(freeMemory());

		// Get the next RI to be rendered
		riBuffer.ClearBuffer();			// Need to clear or otherwise we can end up with unexpected junk
		bool hasInstruction = lpe.GetNextRI(&riBuffer);

		if (hasInstruction) {
			unsigned long endGetRi = millis() - currentMillis;
			Serial.print("End get RI ");
			Serial.print(millis());
			Serial.print(".  Duration = ");
			Serial.print(endGetRi);
			Serial.print(".  Next RI = ");
			Serial.print(riBuffer.GetBuffer());
			Serial.print(".  Free memory = ");
			Serial.println(freeMemory());

			// Render the instruction
			unsigned long startExecute = millis();
			render.SetRI(riBuffer.GetBuffer());
			render.Render();
			Serial.print("Rendering took ");
			Serial.println(millis() - startExecute);

		}
		else {
			Serial.println("NO MORE INSTRUCTIONS!!!");
		}

		nextSetRi = currentMillis + RENDERING_FRAME;
	}
}

void loop() {
	SingleProgramTest();
}