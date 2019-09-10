/*
 Name:		Light_Server.ino
 Created:	7/17/2019 8:46:06 PM
 Author:	kevin
*/


/*
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

FixedSizeCharBuffer riBuffer = FixedSizeCharBuffer(1000);
StringProcessor stringProcessor = StringProcessor();
LEDConfig ledConfig = LEDConfig(NUMLEDS);
Adafruit_NeoPixel pixels(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);
IPixelController* pixelController = &pixels;


Renderer render = Renderer(&stringProcessor, &ledConfig, pixelController);


bool firstIns = true;
	// the setup function runs once when you press reset or power the board
void setup() {
	// pinMode(LED_BUILTIN, OUTPUT);
	pixels.begin();


	render.SetRI("00000008");
	render.Render();
}

void loop() {

	TestSliderOutputToRender();
	// TestLPIOutputToRender();
	// TestTimingOfRender()
}

#define SLIDER_NEAR	"03010000020FF000000FF00"
#define SLIDER_FAR "03010000021FF000000FF00"
bool startNear = true;
LPIInstruction lpiIns = LPIInstruction();
FixedSizeCharBuffer lpiBuffer = FixedSizeCharBuffer(1000);
SliderAnimatedLPI* lpi = nullptr;
void TestSliderOutputToRender() {
	currentMillis = millis();

	if (currentMillis >= nextSetRi) {
		Serial.println("Time");


		
		if (lpi == nullptr) {
			lpiIns.opcode = 3; lpiIns.duration = 1; lpiIns.lpi = &lpiBuffer;
			lpiBuffer.ClearBuffer();
			if (startNear)
				lpiBuffer.LoadFromBuffer(SLIDER_NEAR);
			else
				lpiBuffer.LoadFromBuffer(SLIDER_FAR);
			startNear = !startNear;

			lpi = new SliderAnimatedLPI(&lpiIns, &ledConfig, &stringProcessor);
			bool validated = lpi->Validate();
			Serial.println(validated);
		}

		riBuffer.ClearBuffer();
		lpi->GetNextRI(&riBuffer);
		Serial.println(riBuffer.GetBuffer());
		bool nextRi = render.SetRI(riBuffer.GetBuffer());
		render.Render();

		if (!nextRi) delete lpi;

		nextSetRi = currentMillis + 1000;
	}
}


/*
bool currentColourIsRed = 0;
#define SOLID_INS_RED	"01010000FF0000"
#define SOLID_INS_BLUE	"010100000000FF"
LPIInstruction lpiIns = LPIInstruction();
FixedSizeCharBuffer lpiBuffer = FixedSizeCharBuffer(1000);
void TestLPIOutputToRender() {
	currentMillis = millis();

	if (currentMillis >= nextSetRi) {
		Serial.println("Time");

		currentColourIsRed = !currentColourIsRed;
		SolidNonAnimatedLPI* lpi = nullptr;

		lpiIns.opcode = 1; lpiIns.duration = 1; lpiIns.lpi = &lpiBuffer;
		if (currentColourIsRed)
			lpiBuffer.LoadFromBuffer(SOLID_INS_RED);
		else
			lpiBuffer.LoadFromBuffer(SOLID_INS_BLUE);
		lpi = new SolidNonAnimatedLPI(&lpiIns, &ledConfig, &stringProcessor);

		bool validated = lpi->Validate();
		Serial.print(validated);
		lpi->GetNextRI(&riBuffer);

		Serial.println(riBuffer.GetBuffer());
		render.SetRI(riBuffer.GetBuffer());
		render.Render();

		delete lpi;

		nextSetRi += 500;
	}
}
*/

/*
void TestTimingOfRenderer() {
	currentMillis = millis();

	if (currentMillis >= nextSetRi) {
		Serial.print("[START] nextSetRi = ");
		Serial.print(nextSetRi);

		if (firstIns)
			render.SetRI(RI_1);
		else
			render.SetRI(RI_2);

		uint32_t duration = millis() - currentMillis;
		Serial.print("[DURATION] duration = ");
		Serial.print(duration);

		nextSetRi += RENDERING_FRAME;
		Serial.print("[END] nextSetRi = ");
		Serial.println(nextSetRi);

		firstIns = !firstIns;
	}

	if (currentMillis >= nextRender) {
		Serial.print("[START] nextRender = ");
		Serial.print(nextRender);

		render.Render();
		uint32_t duration = millis() - currentMillis;
		Serial.print("[DURATION] duration = ");
		Serial.print(duration);

		nextRender += RENDERING_FRAME;

		Serial.print("[END] nextRender = ");
		Serial.println(nextRender);
	}
}
*/