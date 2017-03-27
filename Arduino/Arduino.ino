// - - - - -
// A demo Application for our little Arduino Library. Please copy the FLIPDOT folder into your Arduino/libraries folder to use the lib
//
//
// Copyright (C) 2016 fluepke, kryptokommunist <fabian.luepke@student.hpi.de>,<marcus.ding@student.hpi.de>
// This work is licensed under a GNU style license.
//
// Last change: kryptokommunist
//
// Documentation and samples are available at https://github.com/nerdmanufaktur/flipdot
// - - - -
#include <ESP8266WiFi.h>
#include "FLIPDOT.h"

const char* ssid = "Mettigel24.de | Get off my LAN";
const char* password = "N00bznet?NoWay!";

FLIPDOT *board = new FLIPDOT();

//NTC variables
#include <WiFiUdp.h>
unsigned int localPort = 2390;      // local port to listen for UDP packets

/* Don't hardwire the IP address or we won't get the benefits of the pool.
 *  Lookup the IP address for the host name instead */
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

void setup() {
  board->init(); //must come first to avoid error state of board
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
}

uint16_t framebuffer[25] = {0, 0b0000001110000000, 0b0000011111000000, 0b0000111111100000, 0b0001111111110000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000,
                            0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000,
                            0b0011111111111000, 0b0001111111110000, 0b0000111111100000, 0b0000011111000000, 0b0000001110000000, 0
                           };

uint16_t framebuffer2[25] = {0, 0, 0, 0, 0, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0, 0, 0, 0, 0};

void loop() {
  board->render_string(getNTPTime().c_str(), 4, ZERO_ALL);
  delay(2000);
  board->render_string(getNTPTime().c_str(), 4,ZERO_ALL);
  delay(1000);
  /*
  board->all_off();
  const float delta = 8*PI/115;
  float rad = 0;
  for (float a = 0; a < 115; a++) {
    float val = 7.6+7.6*sin(rad);
    board->draw_in_internal_framebuffer(1,a,val); 
    board->render_internal_framebuffer();
    rad += delta;
  }
  rad = 0;
  for (float a = 0; a < 115; a++) {
    float val = 7.6+7.6*cos(rad);
    board->draw_in_internal_framebuffer(1,a,val); 
    board->render_internal_framebuffer();
    rad += delta;
  }*/
  /*
  delay(1000);
  for (uint8_t a = 114; a >= 0; a-=1) {
    board->draw_in_internal_framebuffer(1,a,a%15); 
    board->render_internal_framebuffer();
  }
  delay(1000);*/
  board->all_off();
  delay(1000);
  board->all_on();
  delay(1000);
  board->all_off();
  for (int i = 0; i < 2; i++) {
    board->all_on();
    board->all_off();
  }
  delay(1000);
  board->render_string_small("Tagesmenu\0", 4*CHAR_OFFSET_SMALL, 0);
  board->render_string_small("Lauchsuppe\0", 4*CHAR_OFFSET_SMALL, 8,ZERO_NONE);
  board->render_string_small("12:34\0", 0, 4,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("12 34\0", 0, 4,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("12:35\0", 0, 4,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("12 35\0", 0, 4,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("HpyHour\0", 10.4*CHAR_OFFSET_SMALL+1, 0,ZERO_LOCALLY);
  board->render_string_small("Ckts 7\0", 10.4*CHAR_OFFSET_SMALL+1, 8, ZERO_NONE);
  board->render_string_small("12:36\0", 0, 4,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("12 36\0", 0, 4,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("12:37\0", 0, 4,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("12 38\0", 0, 4,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("12:39\0", 0, 4,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("12 39\0", 0, 4,ZERO_LOCALLY);
  board->render_string_small("Tagesmenu\0", 4*CHAR_OFFSET_SMALL, 0);
  board->render_string_small("ist aus!  \0", 4*CHAR_OFFSET_SMALL, 8,ZERO_NONE);
  delay(500);
  board->render_string_small("12:40\0", 0, 4,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("12 41\0", 0, 4,ZERO_LOCALLY);
   delay(500);
  board->render_string_small("12:42\0", 0, 4,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("12:42\0", 0, 4,ZERO_LOCALLY);
  delay(1000);
  board->render_string_small("     \0", 0,ZERO_LOCALLY);
  board->render_string("0\0", 8,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("     \0", 0,ZERO_LOCALLY);
  board->render_string("1\0", 8,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("     \0", 0,ZERO_LOCALLY);
  board->render_string("2\0", 8,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("     \0", 0,ZERO_LOCALLY);
  board->render_string("3\0", 8,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("     \0", 0,ZERO_LOCALLY);
  board->render_string("4\0", 8,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("     \0", 0,ZERO_LOCALLY);
  board->render_string("5\0", 8,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("     \0", 0,ZERO_LOCALLY);
  board->render_string("6\0", 8,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("     \0", 0,ZERO_LOCALLY);
  board->render_string("7\0", 8,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("     \0", 0,ZERO_LOCALLY);
  board->render_string("8\0", 8,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("     \0", 0,ZERO_LOCALLY);
  board->render_string("9\0", 8,ZERO_LOCALLY);
  delay(500);
  board->render_string_small("     \0", 0,ZERO_LOCALLY);
  board->render_string("10\0", 1,ZERO_LOCALLY);
  delay(1000);
  board->scroll_string("Hi");
  delay(1000);
  board->render_string("MORGEN", 4*CHAR_OFFSET-5);
  board->render_string("12:57", 2);
  board->render_string("MORGEN", 4*CHAR_OFFSET-5);
  delay(1000);
  board->render_string("12:58", 2);
  board->render_string("MORGEN", 4*CHAR_OFFSET-5);
  delay(1000);
  board->render_string("13:59", 2);
  board->render_string("MORGEN", 4*CHAR_OFFSET-5);
  delay(1000);
  board->render_string("13:00", 2);
  board->render_string("MORGEN", 4*CHAR_OFFSET-5);
  delay(500);
  board->render_string("13:01", 2);
  board->render_string("MITTAG", 4*CHAR_OFFSET-5);
  delay(1000);
   board->render_string("13:02", 2);
   board->render_string("MITTAG", 4*CHAR_OFFSET-5);
  delay(1000);
   board->render_string("13:03", 2);
   board->render_string("MITTAG", 4*CHAR_OFFSET-5);
   delay(1000);
   board->render_string("13:04", 2);
   board->render_string("MITTAG", 4*CHAR_OFFSET-5);
  delay(1000);
   for (float a = 0; a <= 2 * PI; a += 0.2) {
    int x = 1 + 8 * cos(a);
    int y = 4 + 8 * sin(a);
    board->render_string_small("12:34\0", x, y, ZERO_ALL);
    delay(1);
  }
  for (float a = 2 * PI; a >= 0; a -= 0.2) {
    int x = 1 + 8 * cos(a);
    int y = 4 + 8 * sin(a);
    board->render_string_small("12:34\0", x, y, ZERO_ALL);
  }
float deg = 0;
  for (float a = 0; a <= 115; a++) {
    board->render_string_small("12:34\0", a,5.5+5.5*sin(deg),ZERO_NONE);
    deg += 0.2;
  }
  for (float a = 114; a >= 0; a-=1) {
    board->render_string_small("12:34\0", a, 5.5+5.5*sin(deg));
    deg -= 0.2;
  }
  board->all_off();
  for (int a = 0; a < 115; a++) {
    board->draw_in_internal_framebuffer(1,a,7); 
    board->render_internal_framebuffer();
  }
  delay(1000);
  board->write_to_all_columns(0b0000010000000000);
  delay(1000);
  
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

//get the ntc time string
String getNTPTime(){
  String time = "";
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  
  int cb = udp.parsePacket();
  if (!cb) {
    //Serial.println("no packet yet");
  }
  else {
    //Serial.print("packet received, length=");
    //Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    // now convert NTP time into everyday time:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;

    // print the hour, minute and second:
    // UTC is the time at Greenwich Meridian (GMT)
    time += (epoch  % 86400L) / 3600; // print the hour (86400 equals secs per day)
    time += ':';
    if ( ((epoch % 3600) / 60) < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      time += '0';
    }
    time+=(epoch  % 3600) / 60; // print the minute (3600 equals secs per minute)
    /*
    Serial.print(':');
    if ( (epoch % 60) < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
    */
    return time;
  }
  return "No response";
}

