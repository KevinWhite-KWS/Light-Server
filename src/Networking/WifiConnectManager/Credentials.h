/****************************************************************************************************************************
  Credentials.h for SAMD_WiFiNINA.ino
  For SAMD boards using WiFiNINA Modules/Shields, using much less code to support boards with smaller memory

  WiFiManager_NINA_WM_Lite is a library for the Mega, Teensy, SAM DUE, SAMD and STM32 boards
  (https://github.com/khoih-prog/WiFiManager_NINA_Lite) to enable store Credentials in EEPROM/LittleFS for easy
  configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services without Hardcoding.

  Built by Khoi Hoang https://github.com/khoih-prog/WiFiManager_NINA_Lite
  Licensed under MIT license
 *****************************************************************************************************************************/

#ifndef Credentials_h
#define Credentials_h

#include "defines.h"

 /// Start Default Config Data //////////////////

 /*
 #define SSID_MAX_LEN      32
 //From v1.0.3, WPA2 passwords can be up to 63 characters long.
 #define PASS_MAX_LEN      64
 typedef struct
 {
   char wifi_ssid[SSID_MAX_LEN];
   char wifi_pw  [PASS_MAX_LEN];
 }  WiFi_Credentials;
 #define NUM_WIFI_CREDENTIALS      2
 // Configurable items besides fixed Header, just add board_name
 #define NUM_CONFIGURABLE_ITEMS    ( ( 2 * NUM_WIFI_CREDENTIALS ) + 1 )
 ////////////////
 typedef struct Configuration
 {
   char header         [16];
   WiFi_Credentials  WiFi_Creds  [NUM_WIFI_CREDENTIALS];
   char board_name     [24];
   int  checkSum;
 } WiFiNINA_Configuration;
 */

 /** START: PROTOTYPING ONLY **/
#ifdef DEBUG_MODE
#define TO_LOAD_DEFAULT_CONFIG_DATA      true
#endif
/** END: PROTOTYPING ONLY **/

#if TO_LOAD_DEFAULT_CONFIG_DATA

 // This feature is primarily used in development to force a known set of values as Config Data
 // It will NOT force the Config Portal to activate. Use DRD or erase Config Data with Blynk.clearConfigData()

 // Used mostly for development and debugging. FORCES default values to be loaded each run.
 // Config Portal data input will be ignored and overridden by DEFAULT_CONFIG_DATA
 //bool LOAD_DEFAULT_CONFIG_DATA = true;

 // Used mostly once debugged. Assumes good data already saved in device.
 // Config Portal data input will be override DEFAULT_CONFIG_DATA
#ifdef DEBUG_MODE
bool LOAD_DEFAULT_CONFIG_DATA = true;
#endif

WiFiNINA_Configuration defaultConfig =
{
    //char header[16], dummy, not used
    "WIFININA",
    // WiFi_Credentials  WiFi_Creds  [NUM_WIFI_CREDENTIALS];
    // WiFi_Credentials.wifi_ssid and WiFi_Credentials.wifi_pw






    // *** Enter the WIFI network name in the following string ***
    "MyWifiNetwork",

    // *** Enter the WIFI credentials in the following string ***
    "MyWifiPassword",


    // Leave empty
    "",  "",
    //char board_name     [24];



    // *** Enter the name of the server here
    "MyServerName",



    // terminate the list
    //int  checkSum, dummy, not used
    0
    /////////// End Default Config Data /////////////
};

#else

bool LOAD_DEFAULT_CONFIG_DATA = false;

WiFiNINA_Configuration defaultConfig;

#endif    // TO_LOAD_DEFAULT_CONFIG_DATA

/////////// End Default Config Data /////////////


#endif    //Credentials_h