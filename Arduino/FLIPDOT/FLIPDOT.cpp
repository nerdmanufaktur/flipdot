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
  number_of_panels = sizeof(panel_configuration)/sizeof(panel_configuration[0]);
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

    for(int i = 0; i < number_of_panels; i++){
        uint8_t pin = select_pin_mapping[i];
        //preprocessor function!
        initializePanel(pin)

        for (int j = 0; j < panel_configuration[i]; j++) {
          //preprocessor function!
          writeToNewColumn(columnData,pin)
        }

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
void FLIPDOT::render_frame(uint16_t frame[DISPLAY_WIDTH]) {
  uint8_t current_col = 0;
  for(int i = 0; i < number_of_panels; i++){
      uint8_t pin = select_pin_mapping[i];
      //preprocessor function!
      initializePanel(pin)

      for (int j = 0; j < panel_configuration[i]; j++) {
          writeToNewColumn(frame[current_col++],pin) //future optimization: skip columns!
      }
  }

}

/*
Render internal frame buffer on the board
*/
void FLIPDOT::render_internal_framebuffer() {
  if(frame_buff_changed()){
      render_frame(frame_buff);
  }
}

/*
Render a char to the frame_buff with horizontal offset (can be negative)
*/
void FLIPDOT::render_char_to_buffer(char c, short x_offset) {
  // Convert the character to an index
  c = (c - 32);
  uint16_t current_font_column;
  short current_pos;
  // Draw pixels
  for (uint8_t j=0; j<CHAR_WIDTH; j++) {
    current_pos = x_offset+j;
    if((current_pos >= 0) && (current_pos < DISPLAY_WIDTH)) { //case of negative offset
      current_font_column = pgm_read_word_near(font + c*CHAR_WIDTH + j); //returns uint16_t in big endian
      frame_buff[current_pos] = current_font_column; //converting big endian to little endian for correct column formatting
    }
  }
}

/*
Render a char with 8x8 font to the frame_buff with horizontal/vertical offsets (can be negative)
*/
void FLIPDOT::render_char_to_buffer_small(char c, int x_offset, short y_offset) {
  // Convert the character to an index
  c = (c - 32);
  uint8_t current_font_column;
  uint8_t msb_column;
  uint8_t lsb_column;
  int current_pos;

  if((y_offset > -CHAR_HEIGHT_SMALL) && (y_offset < COL_HEIGHT)) { //check if vertical offset is in visible range
      // Draw pixels
      for (uint8_t j=0; j<CHAR_WIDTH_SMALL; j++) {
        current_pos = x_offset+j;
        if((current_pos >= 0) && (current_pos < DISPLAY_WIDTH)) { //check if horizontal offset is in visible range
          current_font_column = pgm_read_byte_near(font_small + c*CHAR_WIDTH_SMALL + j); //returns uint16_t in big endian
          if(y_offset >= 0 && y_offset < 8) { // y_offset 0 to 7: char spans over both bytes of col
            msb_column = current_font_column << y_offset;
            lsb_column = current_font_column >> (8-y_offset);
          } else if (y_offset >= 0) { // y_offset 8 to 15: char spans only over lower lsb of col
            msb_column = 0;
            lsb_column = current_font_column << (y_offset-8);
          } else { // y_offset -1 to -7: char spans only over upper msb of col
            msb_column = current_font_column >> -y_offset;
            lsb_column = 0;
          }
          frame_buff[current_pos] = msb_column << 8 | lsb_column;  //converting big endian to little endian for correct column formatting
        }
      }
    }
}

/*
Render a string to the flip-dot with horizontal offset (can be negative)
*/
void FLIPDOT::render_string(const char* str, int x_offset = RENDER_STRING_DEFAULT_X_OFFSET) {
    zero_frame_buff();
    while (*str) {
      if(x_offset >= DISPLAY_WIDTH) { //don't try to render invisible chars
        break;
      } else if(x_offset <= -CHAR_WIDTH) {
        str++;
        x_offset += CHAR_OFFSET;
      } else {
        render_char_to_buffer(*str++, x_offset);
        x_offset += CHAR_OFFSET;
      }
    }
    render_internal_framebuffer();
}

/*
Render a string with 8x8 characters to the flip-dot with horizontal and vertical offset (can be negative)
*/
void FLIPDOT::render_string_small(const char* str, int x_offset = RENDER_STRING_DEFAULT_X_OFFSET, short y_offset = DEFAULT_SMALL_Y_OFFSET) {
    zero_frame_buff();
    while (*str) {
      if(x_offset >= DISPLAY_WIDTH) { //don't try to render invisible chars
        break;
      } else if(x_offset <= -CHAR_WIDTH_SMALL) {
        str++;
        x_offset += CHAR_OFFSET_SMALL;
      } else {
        render_char_to_buffer_small(*str++, x_offset, y_offset);
        x_offset += CHAR_OFFSET_SMALL;
      }
    }
    render_internal_framebuffer();
}


/*
Scroll a string over the flip-dot
*/
void FLIPDOT::scroll_string(const char* str, int x_offset = DEFAULT_SCROLL_X_OFFSET, int millis_delay = DEFAULT_SCROLL_DELAY_MILLISECONDS) {
    int str_size = strlen(str);
    int initial_x_offset = x_offset;
    for(int i = 0; i < (str_size*(CHAR_OFFSET)+initial_x_offset); i++) {
        render_string(str, x_offset);
        x_offset--;
        delay(millis_delay);
    }
}

/*
Scroll a small 8x8 font string over the flip-dot
*/
void FLIPDOT::scroll_string_small(const char* str, int x_offset = DEFAULT_SCROLL_X_OFFSET, int millis_delay = DEFAULT_SCROLL_DELAY_MILLISECONDS, short y_offset = DEFAULT_SMALL_Y_OFFSET) {
    int initial_x_offset = x_offset;
    int str_size = strlen(str);
    for(int i = 0; i < (str_size*(CHAR_OFFSET_SMALL)+initial_x_offset); i++) {
        render_string_small(str, x_offset, y_offset);
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

/*
zero the internal frame buffer
*/
void FLIPDOT::zero_frame_buff() {
  memset(frame_buff, 0, DISPLAY_WIDTH*2); //*2 because memset takes no of bytes
}

/*
returns true if the frame_buff changed compared to last_frame_buff
*/
bool FLIPDOT::frame_buff_changed() {
  for(int i = 0; i < DISPLAY_WIDTH; i++) {
    if(frame_buff[i] != last_frame_buff[i]){
      memcpy(&last_frame_buff, &frame_buff, sizeof(frame_buff));
      return true;
    }
  }
  return false;
}
