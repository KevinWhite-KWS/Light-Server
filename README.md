# Arduino LED Light-Server

### What is it?

Control WS2812B LEDs with ease via a powerful RESTful server running on Arduino. Unleash creativity by executing complex programs with a simple file upload in Light Definition Language (LDL).




##### Example: switch between red / blue every 1 second forever:

![image](https://user-images.githubusercontent.com/54847787/217588972-66d0db92-c271-4d39-9d69-66580a353f98.png)





##### Example: a more complex Christmas lights program

![image](https://user-images.githubusercontent.com/54847787/217590452-b579e7b0-82f1-472f-9f83-78df2861a040.png)



### Features

* 




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
