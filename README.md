# flipdot
Contains Code for running the flip-dot displays based on the driver pcb both from nerdmanufaktur.berlin

## Arduino [![Build Status](https://travis-ci.org/nerdmanufaktur/flipdot.svg?branch=master)](https://travis-ci.org/nerdmanufaktur/flipdot)

Copy the folders from /Arduino to your local /Arduino/libraries folder and compile/upload the /Arduino/examples example Code. (Wemos for Wifi enabled ESP8266s and Arduino_HW for everything else)

The FLIPDOT library on an ESP8266 also provides an UDP-Server that listens on Port 1234 for data. It will accept DISPLAY_WIDTH*2 byte sized frames and render them immediatly.
