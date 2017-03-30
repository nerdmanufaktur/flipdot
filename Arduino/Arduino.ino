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
#include <FLIPDOT.h>
#include <ESPTime.h>

#define SSID "Mettigel24.de | Get off my LAN"
#define PASSWORD "N00bznet?NoWay!"

FLIPDOT *board = new FLIPDOT();
ESPTime timer = ESPTime();

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  board->init(); //should come after wifi is connected on ESP8266
}

uint16_t framebuffer[25] = {0, 0b0000001110000000, 0b0000011111000000, 0b0000111111100000, 0b0001111111110000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000,
                            0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000,
                            0b0011111111111000, 0b0001111111110000, 0b0000111111100000, 0b0000011111000000, 0b0000001110000000, 0
                           };

uint16_t framebuffer2[25] = {0, 0, 0, 0, 0, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0, 0, 0, 0, 0};

void loop() {
  board->render_string("4444\0");
  delay(1000);
  board->render_string(timer.getFormattedTime(), 4, ZERO_ALL);
  delay(1000);
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
    board->render_string_small(timer.getFormattedTime(), x, y, ZERO_ALL);
  }
  for (float a = 2 * PI; a >= 0; a -= 0.2) {
    int x = 1 + 8 * cos(a);
    int y = 4 + 8 * sin(a);
    board->render_string_small(timer.getFormattedTime(), x, y, ZERO_ALL);
  }
float deg = 0;
  for (float a = 0; a <= DISPLAY_WIDTH; a++) {
    board->render_string_small(timer.getFormattedTime(), a,5.5+5.5*sin(deg),ZERO_NONE);
    deg += 0.2;
  }
  for (float a = DISPLAY_WIDTH-1; a >= 0; a-=1) {
    board->render_string_small(timer.getFormattedTime(), a, 5.5+5.5*sin(deg));
    deg -= 0.2;
  }
  board->all_off();
  for (int a = 0; a < DISPLAY_WIDTH; a++) {
    board->draw_in_internal_framebuffer(1,a,7); 
    board->render_internal_framebuffer();
  }
  delay(1000);
  board->render_string(timer.getFormattedTime(), 4, ZERO_ALL);
  delay(1000);
}
