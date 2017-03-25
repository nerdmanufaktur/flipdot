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
  board->render_string("HI IHR SPASTEN\0", 4);
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
  }
  board->all_off();
  for (int a = 0; a < 115; a++) {
    board->draw_in_internal_framebuffer(1,a,7); 
    board->render_internal_framebuffer();
  }
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
  board->render_string_small("12:34\0", 0, 4);
  delay(500);
  board->render_string_small("12 34\0", 0, 4);
  delay(500);
  board->render_string_small("12:35\0", 0, 4);
  delay(500);
  board->render_string_small("12 35\0", 0, 4);
  delay(500);
  board->render_string_small("12:36\0", 0, 4);
  delay(500);
  board->render_string_small("12 36\0", 0, 4);
  delay(1000);
  board->render_string("0\0", 8);
  delay(1000);
  board->render_string("1\0", 8);
  delay(1000);
  board->render_string("2\0", 8);
  delay(1000);
  board->all_off();
  board->render_string("3\0", 8);
  delay(1000);
  board->all_off();
  board->render_string("4\0", 8);
  delay(1000);
  board->all_off();
  board->render_string("5\0", 8);
  delay(1000);
  board->all_off();
  board->render_string("6\0", 8);
  delay(1000);
  board->all_off();
  board->render_string("7\0", 8);
  delay(1000);
  board->all_off();
  board->render_string("8\0", 8);
  delay(1000);
  board->all_off();
  board->render_string("9\0", 8);
  delay(1000);
  board->render_string("10\0", 4);
  delay(1500);
  board->all_off();
  board->scroll_string("FlipDot>LUV");
  delay(1000);

  /*
    for(int  x= -4; x < 18; x++) {
     for(int y = -7; y < 16; y++) {
     board->render_string_small("12:34\0",x,y);
    }
    }
    for(int y = -7; y < 16; y++) {
    for(int  x= -4; x < 18; x++) {
     board->render_string_small("12:34\0",x,y);
    }
    }
  */

  board->render_string_small("12:34\0");
  delay(2000);
  board->render_string_small("23:49\0", 0, 4);
  delay(2000);
  board->render_string_small("23:50\0", 0, 4);
  delay(2000);
  board->render_string_small("23:49\0", 0, 8);
  delay(2000);
  for (int i = 0; i < 20; i++) {
    board->render_frame(framebuffer);
    delay(i * 5);
    board->render_frame(framebuffer2);
    delay(i * 5);
  }
  delay(1000);
  board->render_string("13:42", 2);
  delay(1000);
  board->all_off();
  delay(100);
  board->render_string("13:43", 2);
  delay(1000);
  board->all_off();
  delay(100);
  board->render_string("13:43", 2);
  delay(1000);
  board->all_off();
  delay(100);
  board->render_string("12:00", 2);
  delay(1000);
  board->all_off();
  delay(100);
  board->render_string("66:00", 2);
  delay(1000);
}

