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


#ifndef FLIPDOT_h
#define FLIPDOT_h

// ---- Constants/Preprocessor functions ----

#define CHAR_WIDTH 16
#define CHAR_HEIGHT 16
#define CHAR_OFFSET CHAR_WIDTH - 6

#define DEFAULT_SCROLL_DELAY_MILLISECONDS 200

//width of panel (either 20 or 25)
#define ROW_WIDTH 25
#define SHIFT_OE_PIN 9
#define SHIFT_RCK_PIN 3

// Positions in Control Buffer
// |     7    |     6    |     5    |     4    |     3    |     2    |     1    |     0
// |  CLEAR   |   CLOCK  |   RESET  | SELECT 1 | SELECT 2 | SELECT 3 | SELECT 4 | SELECT 5 |

#define CLEAR 7
#define CLOCK 6
#define RESET 5
#define SELECT1 4

#define ALL_COLUMN_PIXELS_ON 0b1111111111111111

//sets the control bits in controlBuffer
#define setControlBits(clr,clk,rst,slct) controlBuffer =  (clr << CLEAR) | (clk << CLOCK) | (rst << RESET) | (slct << SELECT1)

//function call is too slow and the compiler not clever enough, therefore we use a preprocessor fake function
#define initializePanel()  columnBuffer = 0;\
  setControlBits(0,1,0,1);\
  writeToRegisters();\
  digitalWrite(SHIFT_OE_PIN, LOW);\
  delay(2);\
  setControlBits(1,1,0,1);\
  columnBuffer = ALL_COLUMN_PIXELS_ON;\
  writeToRegisters();\
  delayMicroseconds(200);\
  setControlBits(0,1,0,1);\
  columnBuffer = 0;\
  writeToRegisters();\
  delayMicroseconds(100);\
  setControlBits(0,1,1,0);\
  writeToRegisters();\
  delayMicroseconds(200);\
  setControlBits(1,0,1,0);\
  writeToRegisters();\
  delayMicroseconds(20);\
  setControlBits(1,1,1,1);\
  writeToRegisters();\
  delayMicroseconds(30);\
  setControlBits(0,1,1,1);\
  writeToRegisters();

//function call is too slow and the compiler not clever enough, therefore we use a preprocessor fake function
#define writeToNewColumn(data) delayMicroseconds(800);\
    setControlBits(1,1,1,1);\
    writeToRegisters();\
    delayMicroseconds(50);\
    setControlBits(1,0,1,1);\
    writeToRegisters();\
    delayMicroseconds(20);\
    setControlBits(1,1,1,1);\
    columnBuffer = data;\
    writeToRegisters();\
    delayMicroseconds(800);\
    columnBuffer = 0;\
    setControlBits(0,1,1,1);\
    writeToRegisters();\
    delayMicroseconds(50);\
    setControlBits(1,0,1,1);\
    writeToRegisters();\
    delayMicroseconds(10);\
    setControlBits(1,1,1,1);\
    writeToRegisters();\
    delayMicroseconds(20);\
    setControlBits(0,1,1,1);\
    writeToRegisters();

// ---- Methods ----

class FLIPDOT {

//frame buffer (for super fancy diffing etc.)
uint16_t frame_buff[ROW_WIDTH] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint16_t columnBuffer = 0; //holds data of current column pixel data
byte controlBuffer = 0; //holds data of current control bits (clear, clock, reset, select 1, ... , select 5)

public:
  FLIPDOT();
  void FLIPDOT::init();
  void FLIPDOT::writeToAllColumns(uint16_t columnData);
  void FLIPDOT::render_frame(uint16_t frame[ROW_WIDTH]);
  void FLIPDOT::render_string(const char *s, int x_offset);
  void FLIPDOT::scroll_string(const char *s, int millis_delay = DEFAULT_SCROLL_DELAY_MILLISECONDS);
private:
  void FLIPDOT::writeToRegisters();
  void FLIPDOT::render_char_to_buffer(char c, int x);
};

#endif
