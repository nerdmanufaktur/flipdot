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

// ---- Options ----
//flip screen? comment out for deactivating and vice versa
#define TURNDISPLAY_180_DEGREES

//comment out to disable debugging information via Serial
//#define DBG

#include <inttypes.h>
#include <Arduino.h>
#include <SPI.h>
#include "bitreverse_table.h"

//Arduino hw vs. ESP8266
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    #include "font.h"
    #include "font8x8.h"
    #define SHIFT_OE_PIN 9
    #define SHIFT_RCK_PIN 3
#else
    #include "font_non_arduino_hw.h"
    #include "font8x8_non_arduino_hw.h"
    #include <ESP8266WiFi.h>
    #include "ESPAsyncUDP.h"
    #define SHIFT_OE_PIN D2
    #define SHIFT_RCK_PIN D3
#endif



#ifndef FLIPDOT_h
#define FLIPDOT_h

// ---- Constants/Preprocessor functions ----

#define UDP_PORT_FRAME_SERVER 1234

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
#define DEFAULT_ZEROING_SETTING ZERO_LOCALLY

//#define PANEL_CONFIGURATION [5] = {25,25,20,20,25} //widths of panels (either 20 or 25), change arraysize as well!
#define PANEL_CONFIGURATION [1] = {25}
#define MAX_NUMBER_OF_PANELS 5
#define DISPLAY_WIDTH 25
#define COL_HEIGHT 16

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

#if defined(DBG)
  #include "Streaming.h"
  #include <bitset>
  #define DBG_OUTPUT_PORT(data) Serial.print(data);
  #define DBG_OUTPUT_PORT_NL(data) Serial.println(data);
#else
  #define DBG_OUTPUT_PORT(data)
  #define DBG_OUTPUT_PORT_NL(data)
#endif

//sets the control bits in controlBuffer, select for all pins is 1 and for pin select_number it is set to select_value
#define setControlBits(clr,clk,rst,select_value, select_number) controlBuffer =  (clr << CLEAR) | (clk << CLOCK) | (rst << RESET) | (1 << SELECT1) | \
  (1 << SELECT2) | (1 << SELECT3) | (1 << SELECT4) | (1 << SELECT5);\
  /*catch if selected pin is set (its 0 because of inverted transistor output)*/\
  if(select_value == 0){\
      controlBuffer &=  -1 ^ (1 << select_number);\
  }

  //function call is too slow and the compiler not clever enough, therefore we use a preprocessor fake function
  #define initializePanel(select_pin)  columnBuffer = 0;\
    setControlBits(0,1,0,1,select_pin)\
    writeToRegisters();\
    digitalWrite(SHIFT_OE_PIN, LOW);\
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
  #define writeToNewColumn(data,select_pin) delayMicroseconds(1100);\
      setControlBits(1,1,1,1,select_pin)\
      writeToRegisters();\
      setControlBits(1,0,1,1,select_pin)\
      writeToRegisters();\
      setControlBits(1,1,1,1,select_pin)\
      columnBuffer = data;\
      writeToRegisters();\
      delayMicroseconds(800);\
      columnBuffer = 0;\
      setControlBits(0,1,1,1,select_pin)\
      writeToRegisters();\
      setControlBits(1,0,1,1,select_pin)\
      writeToRegisters();\
      setControlBits(1,1,1,1,select_pin)\
      writeToRegisters();\
      setControlBits(0,1,1,1,select_pin)\
      writeToRegisters();

// ---- Methods ----

//ZERO_ALL => zero the entire buffer before rendering
//ZERO_LOCALLY => zero the entire column if something is rendered on it, leave remaining columns as before
//ZERO_NONE => don't zero anything, just merge the previous pixel state with the additional pixels
typedef enum { ZERO_ALL,ZERO_LOCALLY,ZERO_NONE } ZeroOptionsType_t;

class FLIPDOT {

public:
  FLIPDOT();
  void init();
  void render_frame(uint16_t frame[DISPLAY_WIDTH]);
  void render_to_panel(uint16_t* frame, uint8_t panel_index);
  void render_string(const char *s, int x_offset = RENDER_STRING_DEFAULT_X_OFFSET, ZeroOptionsType_t zero_buffer = DEFAULT_ZEROING_SETTING);
  void render_string_small(const char* str, int x_offset = RENDER_STRING_DEFAULT_X_OFFSET, short y_offset = DEFAULT_SMALL_Y_OFFSET, ZeroOptionsType_t zero_buffer = DEFAULT_ZEROING_SETTING);
  void scroll_string(const char *s, int x_offset = DEFAULT_SCROLL_X_OFFSET, int millis_delay = DEFAULT_SCROLL_DELAY_MILLISECONDS);
  void scroll_string_small(const char *s, int x_offset = DEFAULT_SCROLL_X_OFFSET, int millis_delay = DEFAULT_SCROLL_DELAY_MILLISECONDS, short y_offset = DEFAULT_SMALL_Y_OFFSET);
  void render_char_to_buffer(char c, short x, ZeroOptionsType_t zero_buffer);
  void render_char_to_buffer_small(char c, int x, short y_offset, ZeroOptionsType_t zero_buffer);
  void merge_columns(uint16_t* dest_column, const uint16_t* src_column) ;
  void render_internal_framebuffer();
  void draw_in_internal_framebuffer(int val, uint8_t x, uint8_t y);
  void all_off();
  void all_on();
  uint8_t get_panel_column_offset(uint8_t panel_index);
private:
  uint8_t panel_configuration PANEL_CONFIGURATION;
  uint8_t select_pin_mapping[MAX_NUMBER_OF_PANELS] = {SELECT1,SELECT2,SELECT3,SELECT4,SELECT5};
  uint8_t number_of_panels;
  uint16_t frame_buff[DISPLAY_WIDTH] = {0}; //frame buffer (for super fancy diffing etc.)
  uint16_t last_frame_buff[DISPLAY_WIDTH] = {0};
  uint16_t columnBuffer = 0; //holds data of current column pixel data
  byte controlBuffer = 0; //holds data of current control bits (clear, clock, reset, select 1, ... , select 5)
  void writeToRegisters();
  void set_frame_buff(int val);
  bool frame_buff_changed_for_panel(uint8_t panel_index);
  uint16_t font_column_rendering_convert_endianess(uint16_t current_font_column, short y_offset);

  //only for esp8266
  #if !defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)
    AsyncUDP udp;
    void process_udp_frame(uint8_t* data, size_t length);
  #endif
};

#endif
