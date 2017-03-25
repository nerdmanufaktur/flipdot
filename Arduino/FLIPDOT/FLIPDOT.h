// - - - - -
// FLIPDOT - A Arduino library for controlling an Annex/AEG flip dot display
// FLIPDOT.cpp: Library implementation file
//
// Copyright (C) 2016 fluepke, kryptokommunist <fabian.luepke<at>student.hpi.de>,<marcus.ding<at>student.hpi.de>
// This work is licensed under a GNU style license.
//
// Last change: kryptokommunist
//
// Documentation and samples are available at https://github.com/nerdmanufaktur/flipdot
// - - - - -

#include <inttypes.h>
#include <Arduino.h>
#include "SPI.h"
#include "font.h"
#include "font8x8.h"


#ifndef FLIPDOT_h
#define FLIPDOT_h

// ---- Constants/Preprocessor functions ----

#define CHAR_WIDTH 16
#define CHAR_HEIGHT 16
#define CHAR_OFFSET CHAR_WIDTH - 6
#define CHAR_WIDTH_SMALL 8
#define CHAR_HEIGHT_SMALL 8
#define CHAR_OFFSET_SMALL CHAR_WIDTH_SMALL - 3

#define DEFAULT_SCROLL_DELAY_MILLISECONDS 0
#define DEFAULT_SCROLL_X_OFFSET DISPLAY_WIDTH
#define DEFAULT_SMALL_Y_OFFSET 4
#define RENDER_STRING_DEFAULT_X_OFFSET 0

#define PANEL_CONFIGURATION [2] = {25,20} //widths of panels (either 20 or 25), change arraysize as well!
#define MAX_NUMBER_OF_PANELS 5
#define DISPLAY_WIDTH 45
#define ROW_WIDTH 25
#define COL_HEIGHT 16
#define SHIFT_OE_PIN 9
#define SHIFT_RCK_PIN 3

// Positions in Control Buffer
// |     7    |     6    |     5    |     4    |     3    |     2    |     1    |     0
// |  CLEAR   |   CLOCK  |   RESET  | SELECT 1 | SELECT 2 | SELECT 3 | SELECT 4 | SELECT 5 |

#define CLEAR 7
//signal is inverted
#define CLOCK 6
//signal is inverted
#define RESET 5
#define SELECT1 4
#define SELECT2 3
#define SELECT3 2
#define SELECT4 1
#define SELECT5 0

#define ALL_COLUMN_PIXELS_ON 0b1111111111111111

//sets the control bits in controlBuffer, select for all pins is 1 and for pin select_number it is set to select_value
#define setControlBits(clr,clk,rst,select_value, select_number) controlBuffer =  (clr << CLEAR) | (clk << CLOCK) | (rst << RESET) | (1 << SELECT1) | \
  (1 << SELECT2) | (1 << SELECT3) | (1 << SELECT4) | (1 << SELECT5);\
  //catch if selected pin is set (its 0 because of inverted transistor output)
  if(select_value == 0){\
      controlBuffer &=  -1 ^ (1 << select_number);\
  }

//function call is too slow and the compiler not clever enough, therefore we use a preprocessor fake function
#define initializePanel(select_pin)  columnBuffer = 0;\
  setControlBits(0,1,0,1,select_pin)\
  writeToRegisters();\
  digitalWrite(SHIFT_OE_PIN, LOW);\
  delay(3);\
  setControlBits(1,1,0,1,select_pin)\
  columnBuffer = ALL_COLUMN_PIXELS_ON;\
  writeToRegisters();\
  delayMicroseconds(200);\
  setControlBits(0,1,0,1,select_pin)\
  columnBuffer = 0;\
  writeToRegisters();\
  delayMicroseconds(100);\
  setControlBits(0,1,1,0,select_pin)\
  writeToRegisters();\
  delayMicroseconds(200);\
  setControlBits(1,0,1,0,select_pin)\
  writeToRegisters();\
  delayMicroseconds(20);\
  setControlBits(1,1,1,1,select_pin)\
  writeToRegisters();\
  delayMicroseconds(30);\
  setControlBits(0,1,1,1,select_pin)\
  writeToRegisters();

//function call is too slow and the compiler not clever enough, therefore we use a preprocessor fake function
#define writeToNewColumn(data,select_pin) delayMicroseconds(900);\
    setControlBits(1,1,1,1,select_pin)\
    writeToRegisters();\
    delayMicroseconds(100);\
    setControlBits(1,0,1,1,select_pin)\
    writeToRegisters();\
    delayMicroseconds(20);\
    setControlBits(1,1,1,1,select_pin)\
    columnBuffer = data;\
    writeToRegisters();\
    delayMicroseconds(900);\
    columnBuffer = 0;\
    setControlBits(0,1,1,1,select_pin)\
    writeToRegisters();\
    delayMicroseconds(100);\
    setControlBits(1,0,1,1,select_pin)\
    writeToRegisters();\
    delayMicroseconds(10);\
    setControlBits(1,1,1,1,select_pin)\
    writeToRegisters();\
    delayMicroseconds(20);\
    setControlBits(0,1,1,1,select_pin)\
    writeToRegisters();

// ---- Methods ----

class FLIPDOT {

//frame buffer (for super fancy diffing etc.)
uint8_t panel_configuration PANEL_CONFIGURATION;
uint8_t select_pin_mapping[MAX_NUMBER_OF_PANELS] = {SELECT1,SELECT2,SELECT3,SELECT4,SELECT5};
uint8_t number_of_panels;
uint16_t frame_buff[DISPLAY_WIDTH] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint16_t last_frame_buff[DISPLAY_WIDTH] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint16_t columnBuffer = 0; //holds data of current column pixel data
byte controlBuffer = 0; //holds data of current control bits (clear, clock, reset, select 1, ... , select 5)

public:
  FLIPDOT();
  void FLIPDOT::init();
  void FLIPDOT::writeToAllColumns(uint16_t columnData);
  void FLIPDOT::render_frame(uint16_t frame[DISPLAY_WIDTH]);
  void FLIPDOT::render_string(const char *s, int x_offset = RENDER_STRING_DEFAULT_X_OFFSET);
  void FLIPDOT::render_string_small(const char* str, int x_offset = RENDER_STRING_DEFAULT_X_OFFSET, short y_offset = DEFAULT_SMALL_Y_OFFSET);
  void FLIPDOT::scroll_string(const char *s, int x_offset = DEFAULT_SCROLL_X_OFFSET, int millis_delay = DEFAULT_SCROLL_DELAY_MILLISECONDS);
  void FLIPDOT::scroll_string_small(const char *s, int x_offset = DEFAULT_SCROLL_X_OFFSET, int millis_delay = DEFAULT_SCROLL_DELAY_MILLISECONDS, short y_offset = DEFAULT_SMALL_Y_OFFSET);
  void FLIPDOT::render_char_to_buffer(char c, short x);
  void FLIPDOT::render_char_to_buffer_small(char c, int x, short y_offset);
  void FLIPDOT::render_internal_framebuffer();
  void FLIPDOT::all_off();
private:
  void FLIPDOT::writeToRegisters();
  void FLIPDOT::zero_frame_buff();
  bool FLIPDOT::frame_buff_changed();
};

#endif
