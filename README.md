# Arduino LED Light-Server

### What is it?

Control WS2812B LEDs with ease via a powerful RESTful server running on Arduino. Unleash creativity by executing complex programs with a simple file upload in Light Definition Language (LDL).  This project differes from other LED controllers: end users need minimal technical knowledge and user-friendly tools can generate the LDL that can be sent to the server.




##### Example: switch between red / blue every 1 second forever:

![image](https://user-images.githubusercontent.com/54847787/217588972-66d0db92-c271-4d39-9d69-66580a353f98.png)





##### Example: a more complex Christmas lights program

![image](https://user-images.githubusercontent.com/54847787/217590452-b579e7b0-82f1-472f-9f83-78df2861a040.png)

See "Intruction anatomy" for more information regarding individual instructions.



---




### Project goal

Empower creativity and bring lighting designs to life with a user-friendly LED driver! Program stunning light displays using the intuitive Light Definition Language (LDL) - accessible to everyone, regardless of technical expertise.

1. Unleash lighting potential with a standard and powerful Light Definition Language (LDL) for LED control.
2. Empower users of all technical abilities with intuitive tools for crafting stunning LED programs in LDL.
3. Experience seamless LED control with simple HTTP commands to interact with the server.
4. Elevate the LED experience with consumer-friendly touches - including effortless setup via Wifi hotspot, automatic server discovery with UDP, and automatic updates for seamless operation.
5. Bringing light design to all for free with this open-source project.



---



### Where are we and what do we need?

This project represents a significant opportunity and requires the participation of individuals with a strong interest and relevant technical skills (C/C++/Angular/JavaScript). Although the project is currently functional, there is much room for improvement and outstanding issues that need to be addressed. We invite those with the relevant expertise to join us in realizing the full potential of this project.

Top 3 areas needing attention:

1. Address the broken Wifi hotspot setup and improve its functionality.
2. Expand the LED support by integrating more programmable models - beyond the current WS2812B support.
3. Enhance the execution engine with additional instructions for even greater versatility.

If you're eager to contribute to this project, let us know!



---


### Getting Started

*Clone this repository locally:*

``` bash
gh repo clone KevinWhite-KWS/Light-Server
```

1. Open the project using your favorite C++ editor.  The project was built using Visual Studio and the solution file "Light Server.sln" is provided.
2. Connect an Arduino MKR 1010 device.
3. At present the Wifi hotspot functionality does not work.  Therefore you must enter your Wifi credentials until this can be fixed.  Open the file src/Networking/WifiConnectManager/Credentials.h and enter your file credentials on line 69.
3. Build and upload the project to the Arduino.
4. Check it is up and running:
   * The onboard LED should be green.
   * Send the following request: ``` curl -u Super:1xYa1man2* http://192.168.7.226/about ``` and you should see a JSON response e.g. ```{
  "LEDs": 50,
  "LS Version": "1.0.0",
  "LDL Version": "1.0.0",
  "Server Name": "LDL-Bottle"
}```

Congratulations!  You have successfully setup a light server.



---



### Overview

RESTful server to drive WS2812B LEDS.  Light-server programs are writtern in Light Definition Language (LDL) using JSON.




Features:

* UDP server network discovery
* Light Definition Language (LDL) permits complex programs
* WiFi set up mode displays a simple web page to the user on 192.168.4.1:
  * allow WiFi SSID to be selected and password entered
  * allow a 2nd WiFi SSID
  * specify number of connected LEDs
* WiFi automatically re-connects or joins 2nd SSID
* 8 major rendering effects presently supported by LDL
* RESTful interface supports 8 commands
* an LDL program can be saved to device memory; it will reload even after the device has been restarted.

# Related projects
* LDL Light-Server Unit Testing (https://github.com/KevinWhite-KWS/Light-Server-Unit-Tests) : unit tests for this project
* LDL Program Editor (https://github.com/KevinWhite-KWS/Light-Server-Front-End) : blockly editor allowing LDL programs to be visually created and upload to a light server
* LDL Blockly: https://github.com/KevinWhite-KWS/Light-Server-Blockly : blockly customisations used in the LDL Program Editor project



Other, similiar projects:

https://github.com/dannydi12/pi-led-server
https://github.com/tom-2015/rpi-ws2812-server

#### Development opportunities

There are many and we are only limited by our imaginations but there are a few priorities!

Priorities:

1. Address the broken Wifi hotspot setup and improve its functionality.
2. Expand the LED support by integrating more programmable models - beyond the current WS2812B support.
3. Enhance the execution engine with additional instructions for even greater versatility.

Other opportunities:

1. Examine whether it is worth porting or re-writing the project for the Raspberry Pi Pico W (https://www.raspberrypi.com/products/raspberry-pi-pico/).  This device can execute either MicroPython or C/C++.  It is far more capable compared to the MKR 1010 (264kB RAM versus 32 kB, 2 cores versus 1) and much cheaper.  Is it worth the effort?  Much of the code may not require much modification but there will be some Arduino dependencies that need eliminating.
2. OTA updates.  Essential these days but I have no idea how to achieve that.  The update process will need to retain or migrate configuration values and the last uploaded program.
3. Synchronise activity between one or more devices running on the local network.  This could be a master/slave arrangement with all devices synchronised to a master.
4. Integrate with smart devices such as Alexa.  "Alexa, show a fiery pattern on my light devices".
5. Further rendering enhancements such as:
   * Virtual pixel size: instead of 1 LED being a pixel, specify n LEDs being a pixel.  Could add an interesting new dynamic.
   * Virtual LED length: in 100 pixels, specify a length of 50 for example.  We could then specify that the rendering is mirrored on both virtual lengths or some or effect.
6. User friendly tools for generating LDL programs.  The current LDL editor would be a 'power' users tool.  We need tools that can be used by everyone.  Example: how about using AI to generate programs?  A user says or specifies: "a pattern to make me feel warm" and the system generates a 'pleasing' program with reds/yellows etc.  Existing programs can be discoverable on a website and downloaded to devices from the web.
