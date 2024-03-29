#ifndef _UtilityFunctions_h
#define _UtilityFunctions_h

#include "src/Networking/WifiConnectManager/defines.h"

/*
// original PIN assignments: makes an active connection red!  Pins must have changed
// as some point to the ones below.
#define ONBOARD_LED_GREEN_PIN		25
#define	ONBOARD_LED_RED_PIN			26
#define ONBOARD_LED_BLUE_PIN		27
*/
#define	ONBOARD_LED_RED_PIN			25
#define ONBOARD_LED_GREEN_PIN		26
#define ONBOARD_LED_BLUE_PIN		27
void setOnboardRgbLed(uint8_t red, uint8_t green, uint8_t blue) {
#ifdef MKR1010
	WiFiDrv::analogWrite(ONBOARD_LED_RED_PIN, red);   //RED
	WiFiDrv::analogWrite(ONBOARD_LED_GREEN_PIN, green); //GREEN
	WiFiDrv::analogWrite(ONBOARD_LED_BLUE_PIN, blue);   //BLUE
#endif
}

void initOnboardRgbLed() {
#ifdef MKR1010
	WiFiDrv::pinMode(ONBOARD_LED_RED_PIN, OUTPUT);
	WiFiDrv::pinMode(ONBOARD_LED_GREEN_PIN, OUTPUT);
	WiFiDrv::pinMode(ONBOARD_LED_BLUE_PIN, OUTPUT);
	setOnboardRgbLed(0, 0, 0);
#endif
}




bool checkWifistatus(WiFiManager_NINA_Lite* wifiManager)
{
	static unsigned long checkstatus_timeout = 0;
	bool wifiConnected = true;

#define HEARTBEAT_INTERVAL    10000L
	if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
	{
		if (!wifiManager->isConfigDataValid()) {
			// orange = wifi set up mode
			setOnboardRgbLed(255, 165, 0);
			wifiConnected = false;
		} else if (WiFi.status() == WL_CONNECTED) {
			// green = connected to Wifi
			setOnboardRgbLed(0, 255, 0);
		}
		else {
			// red = not connected to Wifi
			setOnboardRgbLed(255, 0, 0);
			wifiConnected = false;
		}
		checkstatus_timeout = millis() + HEARTBEAT_INTERVAL;
	}

	return wifiConnected;
}



#endif