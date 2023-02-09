# Arduino LED Light-Server

### What is it?

Control WS2812B LEDs with ease via a powerful RESTful server running on Arduino. Unleash creativity by executing complex programs with a simple file upload in Light Definition Language (LDL).




##### Example: switch between red / blue every 1 second forever:

![image](https://user-images.githubusercontent.com/54847787/217588972-66d0db92-c271-4d39-9d69-66580a353f98.png)





##### Example: a more complex Christmas lights program

![image](https://user-images.githubusercontent.com/54847787/217590452-b579e7b0-82f1-472f-9f83-78df2861a040.png)

See "Intruction anatomy" for more information regarding individual instructions.




#### Project goal

Empower creativity and bring lighting designs to life with a user-friendly LED driver! Program stunning light displays using the intuitive Light Definition Language (LDL) - accessible to everyone, regardless of technical expertise.

1. Unleash lighting potential with a standard and powerful Light Definition Language (LDL) for LED control.
2. Empower users of all technical abilities with intuitive tools for crafting stunning LED programs in LDL.
3. Experience seamless LED control with simple HTTP commands to interact with the server.
4. Elevate the LED experience with consumer-friendly touches - including effortless setup via Wifi hotspot, automatic server discovery with UDP, and automatic updates for seamless operation.
5. Bringing lifht design to all for free with this open-source project.





### Where are we and what do we need?

This project represents a significant opportunity and requires the participation of individuals with a strong interest and relevant technical skills (C/C++/Angular/JavaScript). Although the project is currently functional, there is much room for improvement and outstanding issues that need to be addressed. We invite those with the relevant expertise to join us in realizing the full potential of this project.

Top 3 areas needing attention:

1. Address the broken Wifi hotspot setup and improve its functionality.
2. Expand the LED support by integrating more programmable models - beyond the current WS2812B support.
3. Enhance the execution engine with additional instructions for even greater versatility.

If you're eager to contribute to this project, let us know!



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

# Related projects
* LDL Light-Server Unit Testing:
* LDL Program Editor:
* LDL Blockly:



Other, similiar projects:

https://github.com/dannydi12/pi-led-server
https://github.com/tom-2015/rpi-ws2812-server
