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



## Overview

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

#### Related projects
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
7. Execute an interrupt program: allow an interrupt program to briefly interrupt the main executing program and render.  This could be useful for rendering things like email notifications but then allowing the main program to continue after.  The additional capabilities of a device like the Raspberry Pi Pico W may be better suited to this enhancemnet.


---



#### What is Light Definition Language?
Light Definition Language is a simple programming language which can be used to create light programs.  Light programs are created in JSON and can be sent to the a light server.

Simple light program example:
```json
{
  "name": "Red / blue",
  "instructions": [
    {
      "repeat": {
        "times": 0,
        "instructions": [
          "01280000FF0000",
          "012800003333FF"
        ]
      }
    }
  ]
}
```

The name property is simply the name of the program.  This is followed by a collecton of one or more instructions:

```json
  "instructions": [
    {
      "repeat": {
        "times": 0,
        "instructions": [
          "01280000FF0000",
          "012800003333FF"
        ]
      }
    }
  ]
```

In this particular case there is only one instruction: a loop instruction:

```json
    {
      "repeat": {
        "times": 0,
        "instructions": [
          "01280000FF0000",
          "012800003333FF"
        ]
      }
    }
```

The loop is instructed to repeat infinitely (```"times" : 0```).  It then has a collection or one or more instructions that will be looped over:

```json
        "instructions": [
          "01280000FF0000",
          "012800003333FF"
        ]
```

Two simple instructions will be executed:

1. ```"01280000FF0000"```: LEDs are activated to red for 1 second (40 frames).
2. ```"012800003333FF"```: LEDs are activated to blue for 1 second (40 frames).

These two instructions repeat infinitely.

##### Rendering frames

The LEDs render at 40 Hz.  That is, a rendering frame is 25ms.  In the example program above both instructions have a duration value of 40 rendering frames.  So that works out to be: 40 * 25 = 1000ms or 1 second.

##### Instruction anatomy

Individual instructions are encoded as follows:

```iiddxxyy{data}```

* ```ii```: Two byte hexadecimal number which specifies the op-code of the instruction that has a valid value between 0x00 (0) and 0xFF (255).
* ```dd```: Two byte hexadecimal number which specifies the duration of the instruction when rendered and has a value between 0x01 (1) and 0xFF (255.  This value is specified in terms of rendering frames.  Example values:
    *     01: the instruction will be rendered for a single frame.
    *     02: the instruction will be rendered for two frames. 
    *     05: the instruction will be rendered for five frames.
    *     64: the instruction will be rendered for one hundred frames.
* ```xx```: Two byte hexadecimal number which is reserved for future use.  It must have a value of 0x00 (0).
* ```yy```: Two byte hexadecimal number which is reserved for future use.  It must have a value of 0x00 (0).
* ```{data}```: A sequence of zero or more bytes that specify the values of the instruction.  For example: a 6-byte hexadecimal number representing the RGB colour to be rendered for the solid instruction code.

##### Current instruction set

| Instruction (**ii**) | Description | Example
| --- | --- | ---
| 00 | Clears the LEDs by turning them all off | ```00010000```
| 01 | Sets all LEDs to the same colour | ```0101000000FF00``` Sets all LEDs to full intensity blue for a single rendering frame.
| 02 | Sets the first n LEDs to a pattern which is then repeated along the entire length of LEDs | ```020500000301020100FF00FF000000FF00``` Specifies a pattern consisting of 3 elements and spanning 4 LEDs in total.  The first LED is set to green.  The second and third LEDs are set to red.  The remaining fourth LED is set to green.  This pattern is then repeated along the entire length of LEDs.
| 03 | Moves a slider (a length of pixels of a specified colour) gradually along the length of pixels | ```030500000200000FF000000FF00``` Specifies that a red slider should be displayed on a green background and that the slider should occupy two pixels of the available pixels.  The slider is rendered beginning from the near side.  Each rendered frame will last for 5 rendering frames.  There should be no head nor tail rendered.
| 04 | Fades in or fades out the LEDs from the start colour to the end colour | ```04010000320000000FFFFFF``` Specifies that the LEDs will fade-in from black to red in a 50 step increment.
| 05 | Randomly sets the LEDs to two or more colours. | ```0505000002FF0000000000``` Specifies that the LEDs should be set to red and black in a stochastic manner.  The effect is rendered for 5 rendering frames.
| 06 | Sets the LEDs to two or more blocks of colours where each block occupies a particular percentage of the available LEDs | ```0601000003212221FF000000FF000000FF``` Specifies that the LEDs should be divided into three groups: 33% red, 34% green, 33% blue.  The effect last for a single rendering frame in duration.
| 07 | Renders a moving ‘rainbow’ colour effect over a number of steps across a specified length | ```070100000A3C003FF000000FF000000FF``` Specifies that a rainbow effect involving red, green, and blue will be rendered over a ‘virtual’ length of 10 pipels for 60 steps.  The effect will start at the end closest to the controller.

See the section "Further documentation" for more comprehensive information about the instruction set.

##### Colours
Colours are specified as 6 hexidecimal values: two for each component of RGB.  For example: ```00FF00``` specifies blue.


---


### What is the RESTful API?

The RESTful server implements the following interface:

| Endpoint | Description
| --- | ---
| GET /power | Gets whether any LEDs are turned on.<br/><br/>Returns: 200 (OK)<br/>```{ “state” : “on” }``` at least one LED is on</br>```{ “state” : “off” }``` all LEDs are presently off
| POST /power/on | Turns on all LEDs to white if no valid colour is specified in the body.  IF a valid colour is specified then the LEDs are set to that colour.  The colour is specified as a simple RRGGBB value in the body.  For example: sending FF0000 in the body will set all LEDs to red.<br/><br/>Returns: 204 (No Content)
| POST /power/off | Turns off all LEDs.<br/><br/>Returns: 204 (No Content)
| POST /program | Validates a light program and, if valid, executes it on the light server.<br/><br/>Returns: 204 (No Content) - LDL program is valid and will be executed by the Light Server</br>Returns: 400 (Bad Request) - LDL program is invalid (body contains information concerning how it is invalid)
| POST /program/stored | Validates a light program and, if valid, executes it on the light server.  This program will be stored on the Light Server and executed again even after the it has been reset.  WARNING: this writes the program to the flash memory and there is a limit of about 10K writes.<br/><br/>Returns: 204 (No Content) - LDL program is valid and will be executed by the Light Server</br>Returns: 400 (Bad Request) - LDL program is invalid (body contains information concerning how it is invalid)
| POST /config/leds | Sets the number of connected LEDs. The body of the message should be an integer between 10 - 350.<br/><br/>Returns: 204 (No Content) - Successfully updated the number of connnected LEDs.<br/>Returns: 400 (Bad Request) - posted configuration is invalid<br/>
| GET /about | Gets information about the server, including: no of connected LEDS, LS version, and LDL version.<br/><br/>```Returns: 200 (OK) e.g. { "LEDs": 20, "LS Version": "1.0.0", "LDL Version" : "1.0.0" }```



All requests are authenticated.  An invalid authentication header results in a 401 (Unauthorised) response.  The authentication header should be set to Basic with a username of "Super" and password "1xYa1man2*".  This value is encoded as "U3VwZXI6MXhZYTFtYW4yKg==".

NOTE: a future enhancement is to replace the hard-coded password with something tied to the device itself.

---

### Further documentation

The following incomplete PDF document provides more indepth information regarding the language specification and server design:

[KWS - Specification_ Light Server (LS) & Light Definition Language (LDL).pdf](https://github.com/KevinWhite-KWS/Light-Server/files/10745730/KWS.-.Specification_.Light.Server.LS.Light.Definition.Language.LDL.pdf)
