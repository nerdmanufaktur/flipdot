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
  pinMode(SHIFT_RCK_PIN, OUTPUT);
  pinMode(SHIFT_OE_PIN, OUTPUT);
  digitalWrite(SHIFT_OE_PIN, LOW); // output shift register values
  number_of_panels = sizeof(panel_configuration)/sizeof(panel_configuration[0]);
  DBG_OUTPUT_PORT("Created FLIPDOT. Calculated no of panels: ")
  DBG_OUTPUT_PORT_NL(number_of_panels)
}

/*
Initialize all pins and getting the SPI ready to rock!
*/
void FLIPDOT::init() {
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
}

/*
Write current control and column buffers via SPI to shift registers
*/
void FLIPDOT::writeToRegisters() {
  SPI.transfer(controlBuffer);
  #if defined(_ESP8266_SPIFAST)
  		SPI.write16(columnBuffer);
  #else
  		#if defined(_SPI_MULTITRANSFER)
  		    //last version of ESP8266 for arduino support this
  		    uint8_t pattern[2] = { (uint8_t)(c >> 8), (uint8_t)(c >> 0) };
  		    SPI.writePattern(pattern, 2, (uint8_t)1);
  		#else
  			 SPI.transfer(columnBuffer >> 8);
         SPI.transfer(columnBuffer >> 0);
      #endif
  #endif
  digitalWrite(SHIFT_RCK_PIN, HIGH);
  digitalWrite(SHIFT_RCK_PIN, LOW);
}

/*
Render a frame on the board
*/
void FLIPDOT::render_frame(uint16_t frame[DISPLAY_WIDTH]) {
    for(uint8_t i = 0; i < number_of_panels; i++){
        if(frame_buff_changed_for_panel(i)){
            render_to_panel(frame, i);
            //only for esp8266, give it time to do fancy networking stuff
            #if defined(ESP8266)
              yield();
            #endif
        }
    }
}

/*
Render a frame on the board without using yield to avoid exceptions when the heap is full
*/
void FLIPDOT::render_frame_no_yield(uint16_t frame[DISPLAY_WIDTH]) {
    for(uint8_t i = 0; i < number_of_panels; i++){
        if(frame_buff_changed_for_panel(i)){
            render_to_panel(frame, i);
        }
    }
}

/*
Render internal frame buffer on the board without using yield to avoid exceptions when the heap is full
*/
void FLIPDOT::render_internal_framebuffer_no_yield() {
      render_frame_no_yield(frame_buff);
}

/*
Render a frame from given buffer to specified panel
*/
void FLIPDOT::render_to_panel(uint16_t* frame, uint8_t panel_index) {
    const uint8_t pin = select_pin_mapping[panel_index];
    const uint8_t col_start = get_panel_column_offset(panel_index);
    //preprocessor function!
    initializePanel(pin)

    #if defined(TURNDISPLAY_180_DEGREES)
    for (int current_col = col_start + panel_configuration[panel_index]-1; current_col >= col_start; current_col -= 1) {
        uint8_t a = frame[current_col];
        uint8_t b = frame[current_col] >> 8;
        uint16_t col = BitReverseTable256[a] << 8 | BitReverseTable256[b] << 0;
    #else
    for (uint8_t current_col = col_start; current_col < col_start + panel_configuration[panel_index]; current_col++) {
        uint16_t col = frame[current_col];
    #endif
        DBG_OUTPUT_PORT_NL(std::bitset<16>(col).to_string().c_str());
        DBG_OUTPUT_PORT("Index current_col: ")
        DBG_OUTPUT_PORT_NL(current_col)
        if(col_changed[current_col]){
          writeToNewColumn(col, pin, 800) //future optimization: skip columns!
        } else {
          writeToNewColumn(col, pin, 10)
        }
    }
    DBG_OUTPUT_PORT_NL()
}

/*
Render internal frame buffer on the board
*/
void FLIPDOT::render_internal_framebuffer() {
      render_frame(frame_buff);
}

/*
Render internal frame buffer on the board with a animation stepping through every changed pixel one by one
*/
void FLIPDOT::render_internal_framebuffer_diff_step_animation() {
      /*
        TO-DO: Implement for different patterns not just upper left to lower right;
      */
      render_frame(frame_buff);
      for(int x = 0; x < DISPLAY_WIDTH; x++){
        uint16_t current_col = frame_buff[x];
        uint16_t last_col = last_frame_buff[x];
        if(current_col != last_col){
          for(int y = 0; y < 16; y++){
            uint8_t bit_mask = 1 << y;
            uint8_t current_col_bit = current_col & bit_mask;
            if (current_col_bit != (last_col & bit_mask)) {
              draw_in_internal_framebuffer(current_col_bit,x,y);
              render_internal_framebuffer();
            }
          }
        }
      }
}


/*
Merges two given columns into the dest_column
*/
void FLIPDOT::merge_columns(uint16_t* dest_column, const uint16_t* src_column) {
  *dest_column |= *src_column;
}

/*
Render a char to the frame_buff with horizontal offset (can be negative)
*/
void FLIPDOT::render_char_to_buffer(char c, short x_offset, ZeroOptions zero_buffer) {
  // Convert the character to an index
  c = (c - 32);
  uint16_t current_font_column;
  short current_pos;
  // Draw pixels
  for (uint8_t j=0; j<CHAR_WIDTH; j++) {
    current_pos = x_offset+j;
    if((current_pos >= 0) && (current_pos < DISPLAY_WIDTH)) { //case of negative offset
      current_font_column = pgm_read_word_near(font + c*CHAR_WIDTH + j); //returns uint16_t in big endian
      if(zero_buffer == ZERO_ALL || zero_buffer == ZERO_LOCALLY){
        frame_buff[current_pos] = current_font_column;
      } else {
        merge_columns(&frame_buff[current_pos],&current_font_column);
      }
    }
  }
}

/*
Render a char with 8x8 font to the frame_buff with horizontal/vertical offsets (can be negative)
*/
void FLIPDOT::render_char_to_buffer_small(char c, int x_offset, short y_offset,  ZeroOptions zero_buffer) {
  // Convert the character to an index
  c = (c - 32);
  uint8_t current_font_column;
  int current_pos;

  if((y_offset > -CHAR_HEIGHT_SMALL) && (y_offset < COL_HEIGHT)) { //check if vertical offset is in visible range
      // Draw pixels
      for (uint8_t j=0; j<CHAR_WIDTH_SMALL; j++) {
        current_pos = x_offset+j;
        if((current_pos >= 0) && (current_pos < DISPLAY_WIDTH)) { //check if horizontal offset is in visible range
          current_font_column = pgm_read_byte_near(font_small + c*CHAR_WIDTH_SMALL + j); //returns uint16_t in big endian
          //endianess is diffrent on ESP8266 boards
          #if defined(ESP8266)
              const uint16_t column_data = current_font_column << y_offset;
          #else
              const uint16_t column_data = font_column_rendering_convert_endianess(current_font_column,y_offset);  //converting big endian to little endian for correct column formatting
          #endif
          if(zero_buffer == ZERO_ALL || zero_buffer == ZERO_LOCALLY){
            frame_buff[current_pos] = column_data;
          } else {
            merge_columns(&frame_buff[current_pos],&column_data);
          }
        }
      }
    }
}

/*
Render a string to the flip-dot with horizontal offset (can be negative)
*/
void FLIPDOT::render_string(const char* str, int x_offset, ZeroOptions zero_buffer) {
    DBG_OUTPUT_PORT("Render string: ")
    DBG_OUTPUT_PORT_NL(str)
    if(zero_buffer == ZERO_ALL) {set_frame_buff(0);}
    while (*str) {
      if(x_offset >= DISPLAY_WIDTH) { //don't try to render invisible chars
        break;
      } else if(x_offset <= -CHAR_WIDTH) {
        str++;
        x_offset += CHAR_OFFSET;
      } else {
        render_char_to_buffer(*str++, x_offset, zero_buffer);
        x_offset += CHAR_OFFSET;
      }
    }
    render_internal_framebuffer();
}

/*
Render a string with 8x8 characters to the flip-dot with horizontal and vertical offset (can be negative)
*/
void FLIPDOT::render_string_small(const char* str, int x_offset, short y_offset, ZeroOptions zero_buffer) {
    DBG_OUTPUT_PORT("Render string small: ")
    DBG_OUTPUT_PORT_NL(str)
    if(zero_buffer == ZERO_ALL) {set_frame_buff(0);}
    while (*str) {
      if(x_offset >= DISPLAY_WIDTH) { //don't try to render invisible chars
        break;
      } else if(x_offset <= -CHAR_WIDTH_SMALL) {
        str++;
        x_offset += CHAR_OFFSET_SMALL;
      } else {
        render_char_to_buffer_small(*str++, x_offset, y_offset, zero_buffer);
        x_offset += CHAR_OFFSET_SMALL;
      }
    }
    render_internal_framebuffer();
}


/*
Scroll a string over the flip-dot
*/
void FLIPDOT::scroll_string(const char* str, int x_offset, int millis_delay, ZeroOptions zero_buffer) {
    DBG_OUTPUT_PORT("Scroll string: ")
    DBG_OUTPUT_PORT_NL(str)
    const int str_size = strlen(str);
    const int initial_x_offset = x_offset;
    for(int i = 0; i < (str_size*(CHAR_OFFSET)+initial_x_offset); i++) {
        render_string(str, x_offset, zero_buffer);
        x_offset--;
        delay(millis_delay);
    }
}

/*
Scroll a small 8x8 font string over the flip-dot
*/
void FLIPDOT::scroll_string_small(const char* str, int x_offset, int millis_delay, short y_offset, ZeroOptions zero_buffer) {
    DBG_OUTPUT_PORT("Scroll string small: ")
    DBG_OUTPUT_PORT_NL(str)
    const int initial_x_offset = x_offset;
    const int str_size = strlen(str);
    for(int i = 0; i <= (str_size*(CHAR_OFFSET_SMALL)+initial_x_offset); i++) {
        render_string_small(str, x_offset, y_offset, zero_buffer);
        x_offset--;
        delay(millis_delay);
    }
}

/*
set internal frame buffer to 0
*/
void FLIPDOT::reset_internal_framebuffer() {
  set_frame_buff(0);
}

/*
all dots off
*/
void FLIPDOT::all_off() {
  set_frame_buff(0);
  render_internal_framebuffer();
}

/*
all dots on
*/
void FLIPDOT::all_on() {
  set_frame_buff(-1);
  render_internal_framebuffer();
}

/*
convert endianness of given data
*/
uint16_t FLIPDOT::font_column_rendering_convert_endianess(uint16_t current_font_column, short y_offset) {
    uint8_t msb_column;
    uint8_t lsb_column;
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
    return msb_column << 8 | lsb_column;
}

/*
change a pixel in internal frame buffer at given coordinates
*/
void FLIPDOT::draw_in_internal_framebuffer(int val, uint8_t x, uint8_t y){
  DBG_OUTPUT_PORT_NL("Drawing in internal framebuffer: ")
  // stupid vodoo because of endianness of uint16_t...
  if(x < 0 || x > 114) {
    return;
  }
  if(/*y>7*/true && y<COL_HEIGHT){
    if(val == 1){
      frame_buff[x] |= 1 << (/*23-*/y);
    } else {
      frame_buff[x] |= ~(1 << (/*23-*/y));
    }
  } else if(y>=0) {
    if(val == 1){
      frame_buff[x] |= 1 << (7-y);
    } else {
      frame_buff[x] |= ~(1 << (7-y));
    }
  }

}

/*
change a byte in internal frame buffer at given coordinates
*/
void FLIPDOT::draw_byte_in_internal_framebuffer(uint8_t val, uint8_t x, uint8_t y){
  DBG_OUTPUT_PORT_NL("Drawing in internal framebuffer: ")
  // stupid vodoo because of endianness of uint16_t...
  if(x < 0 || x > 114) {
    return;
  }
  if(y<COL_HEIGHT && y >= 0){
    if(y == 8) {
      frame_buff[x] = (frame_buff[x] & ~(-1 << 8)) | val << y;
    } else {
      frame_buff[x] = (frame_buff[x] & (-1 << 8)) | val << y;
    }
  }

}


/*
fill the internal frame buffer with given value
*/
void FLIPDOT::set_frame_buff(int val) {
  memset(frame_buff, val, DISPLAY_WIDTH*2);
}

/*
returns true if the frame_buff changed compared to last_frame_buff
*/
bool FLIPDOT::frame_buff_changed_for_panel(uint8_t panel_index) {
  bool did_change = false;
  const uint8_t column_offset = get_panel_column_offset(panel_index);
  const uint8_t panel_width = panel_configuration[panel_index];
  for(int i = column_offset; i < (column_offset+panel_width); i++) {
    if(frame_buff[i] != last_frame_buff[i]){
      uint16_t* ptr = frame_buff;
      ptr += i;
      uint16_t* last_ptr = last_frame_buff;
      last_ptr += i;
      memcpy(last_ptr, ptr, 2); //*2 for number of bytes
      col_changed[i] = true;
      did_change = true;
    } else {
      col_changed[i] = false;
    }
  }
  return did_change;
}

/*
returns the offset number for given panels (columns befor the specified panel)
*/
uint8_t FLIPDOT::get_panel_column_offset(uint8_t panel_index) {
    uint8_t column_offset = 0;
    for(int i = 0; i < panel_index; i++){
      column_offset += panel_configuration[i];
    }
    return column_offset;
}

/*
  returns the byte at specified x,y positions
*/
uint8_t FLIPDOT::read_internal_framebuffer(uint8_t x, uint8_t y) {
  return frame_buff[x] << y;
}

//only for esp8266
#if defined(ESP8266)

/*
Start an udp server listening for frames, wait for Wifi connection before calling
*/
void FLIPDOT::start_udp_server() {
  //Initialize ansynchronous UDP server to listen for incoming frames
  if(udp.listen(UDP_PORT_FRAME_SERVER)) {
        DBG_OUTPUT_PORT("UDP Listening on IP: ")
        DBG_OUTPUT_PORT(WiFi.localIP())
        DBG_OUTPUT_PORT(" Port:")
        DBG_OUTPUT_PORT_NL(UDP_PORT_FRAME_SERVER)
        udp.onPacket([&](AsyncUDPPacket packet) {
            DBG_OUTPUT_PORT("UDP Packet Type: ")
            DBG_OUTPUT_PORT(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast")
            DBG_OUTPUT_PORT(", From: ")
            DBG_OUTPUT_PORT(packet.remoteIP())
            DBG_OUTPUT_PORT(":")
            DBG_OUTPUT_PORT(packet.remotePort())
            DBG_OUTPUT_PORT(", To: ")
            DBG_OUTPUT_PORT(packet.localIP())
            DBG_OUTPUT_PORT(":")
            DBG_OUTPUT_PORT(packet.localPort())
            DBG_OUTPUT_PORT(", Length: ")
            DBG_OUTPUT_PORT(packet.length())
            DBG_OUTPUT_PORT(", Data: ")
            DBG_OUTPUT_PORT_NL()
            #if defined(DBG)
              Serial.write(packet.data(), packet.length());
              //reply to the client
              packet.printf("Got %u bytes of data", packet.length());
            #endif
            time_since_last_udp_frame = millis();
            FLIPDOT::process_udp_frame(packet.data(), packet.length());
        });
      }
  }

  /*
  Stop the udp server
  */
  void FLIPDOT::stop_udp_server() {
    udp.close();
  }

  /*
  processes incoming frame received via ansynchronous udp server
  */
  void FLIPDOT::process_udp_frame(uint8_t* data, size_t length) {
    //make sure we got a complete frame
    if(length == DISPLAY_WIDTH*2){
      DBG_OUTPUT_PORT_NL("Rendering UDP server frame")
      memcpy(frame_buff, data, DISPLAY_WIDTH*2);
      render_internal_framebuffer_no_yield();
    }
  }

  /*
  Check if a udp frame was received in the last 3 seconds
  */
  bool FLIPDOT::received_udp_frame_recently() {
    return millis() > time_since_last_udp_frame + 3000;
  }
#endif
