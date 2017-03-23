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

/* ----- LIBRARIES ----- */
#include "FLIPDOT.h"


/*
Initialize to set the pin mapping as described above
*/
FLIPDOT::FLIPDOT() {

}

/*
Initialize all pins and getting the SPI ready to rock!
*/
void FLIPDOT::init() {
  pinMode(SHIFT_RCK_PIN, OUTPUT);
  pinMode(SHIFT_OE_PIN, OUTPUT);
  digitalWrite(SHIFT_OE_PIN, LOW); // output shift register values

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
}

/*
write given column to all columns on panel
*/
void FLIPDOT::writeToAllColumns(uint16_t columnData) {
  //preprocessor function!
  initializePanel()

  for (int i = 0; i < ROW_WIDTH; i++) {
    //preprocessor function!
    writeToNewColumn(columnData)
  }

}

/*
Write current control and column buffers via SPI to shift registers
*/
void FLIPDOT::writeToRegisters() {
  SPI.transfer(controlBuffer);
  SPI.transfer(&columnBuffer, 2); // 2 bytes size
  digitalWrite(SHIFT_RCK_PIN, HIGH);
  digitalWrite(SHIFT_RCK_PIN, LOW);
}

/*
Render a frame on the board
*/
void FLIPDOT::render_frame(uint16_t frame[ROW_WIDTH]) {
  //preprocessor function!
  initializePanel()

  for (int i = 0; i < ROW_WIDTH; i++) {
      writeToNewColumn(frame[i]) //future optimization: skip columns!
  }

}

/*
Render internal frame buffer on the board
*/
void FLIPDOT::render_internal_framebuffer() {
  render_frame(frame_buff);
}


/*
Render a char to the frame_buff with horizontal offset (can be negative)
*/
void FLIPDOT::render_char_to_buffer(char c, uint8_t x_offset) {
  // Convert the character to an index
  c = (c - 32);
  uint16_t current_font_column;
  uint8_t current_pos;
  // Draw pixels
  for (uint8_t j=0; j<CHAR_WIDTH; j++) {
    current_pos = x_offset+j;
    if((current_pos >= 0) && (current_pos < ROW_WIDTH)) { //case of negative offset
      current_font_column = pgm_read_word_near(font + c*CHAR_WIDTH + j); //returns uint16_t in big endian
      frame_buff[current_pos] = (current_font_column >> 8) | (current_font_column << 8); //converting big endian to little endian for correct column formatting
    }
  }
}

/*
Render a string to the flip-dot with horizontal offset (can be negative)
*/
void FLIPDOT::render_string(const char* str, uint8_t x_offset) {
    while (*str) {
        render_char_to_buffer(*str++, x_offset);
        x_offset += CHAR_OFFSET;
    }
    render_internal_framebuffer();
}


/*
Scroll a string over the flip-dot
*/
void FLIPDOT::scroll_string(const char* str, int millis_delay = DEFAULT_SCROLL_DELAY_MILLISECONDS) {
    int x_offset = 0;
    int char_offset = 0;
    const char* str_ptr = str;
    int str_size = strlen(str);
    int current_pos = 0;
    for(int i = 0; i < str_size*(CHAR_OFFSET); i++) {
        char_offset = 0;
        while (*str) {
            current_pos = x_offset+char_offset;
            if(current_pos >= ROW_WIDTH) { //don't try to render invisible chars
              break;
            } else if (current_pos <= -CHAR_WIDTH) {
              str++;
              char_offset += CHAR_OFFSET;
            } else {
              render_char_to_buffer(*str++, current_pos);
              char_offset += CHAR_OFFSET;
            }
        }
        render_internal_framebuffer();
        str = str_ptr;
        x_offset--;
        delay(millis_delay);
    }
}

/*
all dots off
*/
void FLIPDOT::all_off() {
  writeToAllColumns(0b00000000000000);
}
