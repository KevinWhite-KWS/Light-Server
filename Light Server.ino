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
		* 08/08/2019: Add a test class for the ClearNonAnimatedLPI class (methods: Validate, GetTotalNumberOfSteps, GetNextRI)
		* 08/08/2019: Implement ClearNonAnimatedLPI class
		* 08/08/2019: Add a method to StringProcessor to extract an instruction into a structure (ins, duration, x, y)
		* 20/08/2019: Add a test method for the new method to extract an instruction into a structure

	DONE:
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

uint32_t next = millis();
uint32_t interval = 500;
bool turnOn = true;

#define		PIN		6
#define		NUMLEDS	8

#define		RENDERING_FRAME		1000

#define		RI_1	"FF00000100FF0001R"
#define		RI_2	"00FF0001FF000001R"

uint32_t nextSetRi = millis() + 39;
uint32_t nextRender = millis() + 49;
uint32_t currentMillis = millis();


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

StringProcessor stringProcessor = StringProcessor();
LEDConfig ledConfig = LEDConfig(NUMLEDS);

// Values to support rendering
Adafruit_NeoPixel pixels(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);
IPixelController* pixelController = &pixels;
Renderer render = Renderer(&stringProcessor, &ledConfig, pixelController);

// Values to support LP execution
FixedSizeCharBuffer riBuffer = FixedSizeCharBuffer(1000);
FixedSizeCharBuffer lpBuffer = FixedSizeCharBuffer(1000);
LPE lpe = LPE(&ledConfig, &stringProcessor);



/*
ins: solid green (1 frame duration)
ins: solid red (1 frame duration)
*/
// #define LP_BASIC_TEST "{ \"name\" : \"my program\", \"instructions\" : { \"instruction\":\"0101000000FF00\",  \"instruction\":\"01010000FF0000\"} }"
/*
repeat (1..2):
	ins: solid white
	repeat(1..2):
		ins: solid red
		repeat(1..5):
			ins: solid green
			ins: solid blue
		ins: clear (****)
*/
#define LP_COMPLEX_TEST "{ \"name\" : \"my program\", \"instructions\" : { \"repeat\": { \"times\" : 2, \"instructions\" : { \"instruction\" : \"01010000FFFFFF\", \"repeat\" : { \"times\": 2, \"instructions\": { \"instruction\": \"01010000FF0000\", \"repeat\" : { \"times\" : 5, \"instructions\" : { \"instruction\" : \"0101000000FF00\", \"instruction\": \"010100000000FF\" } }, \"instruction\": \"00010000\"  } } } } } }"
/*
repeat (1..5):
	ins: solid green
	ins: solid red
ins: clear
repeat (1..5):
	ins: solid blue
	ins: solid white
ins: clear
repeat (1..5):
	ins: solid (FF,FF,00)
	ins: solid (00,FF,FF) (****)
*/
#define LP_COMPLEX_TEST_2 "{ \"name\" : \"my program\", \"instructions\" : { \"repeat\" : { \"times\" : 5, \"instructions\" : { \"instruction\": \"0101000000FF00\", \"instruction\": \"01010000FF0000\"  } }, \"instruction\" : \"00010000\", \"repeat\" : { \"times\" : 5, \"instructions\" : { \"instruction\": \"010100000000FF\", \"instruction\": \"01010000FFFFFF\"  } }, \"instruction\" : \"00010000\", \"repeat\" : { \"times\" : 5, \"instructions\" : { \"instruction\": \"01010000FFFF00\", \"instruction\": \"0101000000FFFF\"  } } } }"
/*
repeat (infinite):
	ins: fade black to red
	ins: fade red to black
*/
// #define LP_INFINITE_FADE_TEST "{ \"name\" : \"fade test\", \"instructions\" : { \"repeat\" : { \"times\": 0, \"instructions\" : { \"instruction\" : \"04050000320000000FF0000\", \"instruction\" : \"04050000321FF0000000000\" } }}}"
#define LP_INFINITE_FADE_TEST "{ \"name\" : \"fade test\", \"instructions\" : { \"repeat\" : { \"times\": 0, \"instructions\" : { \"instruction\" : \"04010000050000000FF0000\", \"instruction\" : \"04010000051FF0000000000\", \"instruction\" : \"0401000005000000000FF00\", \"instruction\" : \"0401000005100FF00000000\", \"instruction\" : \"040100000500000000000FF\", \"instruction\" : \"040100000510000FF000000\"  } }}}"
/*
repeat (infinite):
	ins: repeat (two red, two green)
	ins: repeat (one green, two red, one green)
	ins: repeat (two green, two red)
	ins: repeat (one red, two green, one red)
*/
#define LP_INFINITE_ANTS_TESTS "{ \"name\" : \"fade test\", \"instructions\" : { \"repeat\" : { \"times\": 0, \"instructions\" : { \"instruction\" : \"02050000020202FF000000FF00\", \"instruction\" : \"020500000301020100FF00FF000000FF00\", \"instruction\" : \"0205000002020200FF00FF0000\", \"instruction\" : \"0205000003010201FF000000FF00FF0000\" } }}}"
/*
repeat (infinite):
	ins: repeat (one red, three green)				:	02050000020103FF000000FF00
	ins: repeat (one green, one red, two green)		:	020500000301010200FF00FF000000FF00
	ins: repeat (two green, one red, one green)		:	020500000302010100FF00FF000000FF00
	ins: repeat (three green, one green)			:	0205000002030100FF00FF0000
*/
#define LP_INFINITE_ANTS_TESTS2 "{ \"name\" : \"fade test\", \"instructions\" : { \"repeat\" : { \"times\": 0, \"instructions\" : { \"instruction\" : \"02050000020103FF000000FF00\", \"instruction\" : \"020500000301010200FF00FF000000FF00\", \"instruction\" : \"020500000302010100FF00FF000000FF00\", \"instruction\" : \"0205000002030100FF00FF0000\" } }}}"
/*
repeat (infinite):
	ins: slider (two red pixels, green background, start near)		:	03050000190FF000000FF00
	ins: slider (two red pixels, green background, start far)		:	03050000191FF000000FF00
*/
#define LP_INFINITE_KNIGHTRIDER "{ \"name\" : \"knight rider test\", \"instructions\" : { \"repeat\" : { \"times\": 0, \"instructions\" : { \"instruction\" : \"03050000020FF000000FF00\", \"instruction\" : \"03050000021FF000000FF00\" } }}}"
/*
repeat (infinite):
	ins: stochastic (red on black)		:	05050000020101FF000000FF00
*/
#define LP_INFINITE_STOCHASTIC "{ \"name\" : \"stochastic test\", \"instructions\" : { \"repeat\" : { \"times\": 0, \"instructions\" : { \"instruction\" : \"0564000002FF0000000000\" } }}}"
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

void ValidateProgram() {
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
}

bool firstProg = true;
	// the setup function runs once when you press reset or power the board
void setup() {
	// pinMode(LED_BUILTIN, OUTPUT);
	pixels.begin();

	// Set the program to be loaded here
	// const char* lp = LP_INFINITE_FADE_TEST;
	// const char* lp = LP_INFINITE_ANTS_TESTS2;
	// const char* lp = LP_INFINITE_KNIGHTRIDER;
	// const char* lp = LP_INFINITE_STOCHASTIC;
	const char* lp = LP_INFINITE_COMPLEX;
	lpBuffer.LoadFromBuffer(lp);

	// Validate the program first
	ValidateProgram();
	
	
	
	// render.SetRI("00000008");
	// render.Render();
}


void ExternalTwoProgramTest() {
	currentMillis = millis();

	if (currentMillis >= nextSetRi) {
		// Output a debug message with start time of op
		unsigned long startExecute = millis();
		Serial.print("Start get RI @ ");
		Serial.print(startExecute);
		Serial.print(".  Free memory = ");
		Serial.println(freeMemory());

		// Get the next RI to be rendered
		bool hasInstruction = lpe.GetNextRI(&riBuffer);

		if (hasInstruction) {
			unsigned long endGetRi = millis() - startExecute;
			Serial.print("End get RI ");
			Serial.print(millis());
			Serial.print(".  Duration = ");
			Serial.print(endGetRi);
			Serial.print(".  Next RI = ");
			Serial.print(riBuffer.GetBuffer());
			Serial.print(".  Free memory = ");
			Serial.println(freeMemory());

			// Render the instruction
			render.SetRI(riBuffer.GetBuffer());
			render.Render();
		}
		else {
			firstProg != firstProg;

			if (firstProg) {
				Serial.println("NO INSTRUCTION TO RENDER - LOADING FIRST PROGRAM");
				lpBuffer.LoadFromBuffer(LP_COMPLEX_TEST);
			}
			else {
				Serial.println("NO INSTRUCTION TO RENDER - LOADING SECOND PROGRAM");
				lpBuffer.LoadFromBuffer(LP_COMPLEX_TEST);
			}
			LPValidateResult result = LPValidateResult();
			lpe.ValidateLP(&lpBuffer, &result);
		}

		nextSetRi = currentMillis + RENDERING_FRAME;
	}
}

void SingleProgramTest() {
	currentMillis = millis();

	if (currentMillis >= nextSetRi) {
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
	// ExternalTwoProgramTest();

	SingleProgramTest();

}