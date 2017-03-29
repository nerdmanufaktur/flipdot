# flipdot
Contains Code for running the flip-dot displays based on the driver pcb both from nerdmanufaktur.berlin

## Arduino
Copy the folders from /Arduino to your local /Arduino/libraries folder and compile/upload the Arduino.ino example Code.

The FLIPDOT library on an ESP8266 also provides an UDP-Server that listens on Port 1234 for data. It will accept DISPLAY_WIDTH*2 byte sized frames and render them immediatly.