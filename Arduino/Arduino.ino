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

#include "FLIPDOT.h"

FLIPDOT *board = new FLIPDOT();

void setup() {
  Serial.begin(115200);
  board->init();
}

uint16_t framebuffer[25] = {0, 0b0000001110000000, 0b0000011111000000, 0b0000111111100000, 0b0001111111110000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000,
                            0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000, 0b0011111111111000,
                            0b0011111111111000, 0b0001111111110000, 0b0000111111100000, 0b0000011111000000, 0b0000001110000000, 0
                           };

uint16_t framebuffer2[25] = {0, 0, 0, 0, 0, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b0000001110000000, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0, 0, 0, 0, 0};

void loop() {
  board->render_string("HALLO WELT!\0", 4);
  delay(1000);
  board->render_string("Ciao DOOF!\0", 4, false);
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
  board->all_off();
  for (int a = 0; a < 115; a++) {
    board->draw_in_internal_framebuffer(1,a,7); 
    board->render_internal_framebuffer();
  }
  delay(1000);
  board->write_to_all_columns(0b0000010000000000);
  delay(1000);
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
  for (float a = 0; a <= 2 * PI; a += 0.2) {
    int x = 1 + 8 * cos(a);
    int y = 4 + 8 * sin(a);
    board->render_string_small("12:34\0", x, y);
  }
  for (float a = 2 * PI; a >= 0; a -= 0.2) {
    int x = 1 + 8 * cos(a);
    int y = 4 + 8 * sin(a);
    board->render_string_small("12:34\0", x, y);
  }
float deg = 0;
  for (float a = 0; a <= 115; a++) {
    board->render_string_small("12:34\0", a,5.5+5.5*sin(deg),false);
    deg += 0.2;
  }
  for (float a = 114; a >= 0; a-=1) {
    board->render_string_small("12:34\0", a, 5.5+5.5*sin(deg));
    deg -= 0.2;
  }
  board->render_string_small("Tagesmenü:\0", 8*CHAR_OFFSET_SMALL, 0);
  board->render_string_small("Brokkolisuppe\0", 8*CHAR_OFFSET_SMALL, 8);
  board->render_string_small("12:34\0", 0, 4,false);
  delay(500);
  board->render_string_small("12 34\0", 0, 4,false);
  delay(500);
  board->render_string_small("12:35\0", 0, 4,false);
  delay(500);
  board->render_string_small("12 35\0", 0, 4,false);
  delay(500);
  board->render_string_small("Happy Hour:\0", 22*CHAR_OFFSET_SMALL, 0);
  board->render_string_small("Cocktails 7€\0", 22*CHAR_OFFSET_SMALL, 8);
  board->render_string_small("12:36\0", 0, 4,false);
  delay(500);
  board->render_string_small("12 36\0", 0, 4,false);
  delay(500);
  board->render_string_small("12:37\0", 0, 4,false);
  delay(500);
  board->render_string_small("12 37\0", 0, 4,false);
  delay(1000);
  board->render_string("0\0", 8);
  delay(500);
  board->render_string("1\0", 8);
  delay(500);
  board->render_string("2\0", 8);
  delay(500);
  board->all_off();
  board->render_string("3\0", 8);
  delay(500);
  board->all_off();
  board->render_string("4\0", 8);
  delay(500);
  board->all_off();
  board->render_string("5\0", 8);
  delay(500);
  board->all_off();
  board->render_string("6\0", 8);
  delay(500);
  board->all_off();
  board->render_string("7\0", 8);
  delay(500);
  board->all_off();
  board->render_string("8\0", 8);
  delay(500);
  board->all_off();
  board->render_string("9\0", 8);
  delay(500);
  board->render_string("10\0", 4);
  delay(1000);
  board->all_off();
  board->scroll_string("Hi");
  delay(1000);
  board->render_string("12:38", 2);
  delay(1000);
  board->render_string("12:39", 2);
  delay(1000);
  board->render_string("13:40", 2);
  delay(1000);
  board->render_string("12:41", 2);
  delay(1000);
  board->render_string("12:42", 2);
  delay(1000);
}

