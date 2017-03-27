# - - - - -
# genfont - A bitmap font file to C array converter
# use a tool like this here to generate a .bmp file with fonts: http://www.codehead.co.uk/cbfg/
#
# Copyright (C) 2016 fluepke, kryptokommunist <fabian.luepke@student.hpi.de>,<marcus.ding@student.hpi.de>
# This work is licensed under a GNU style license.
#
# Last change: kryptokommunist
#
# Documentation and samples are available at https://github.com/nerdmanufaktur/flipdot
# - - - -

import os,sys,struct
import numpy as np

#just put the one you want to generate last
FILE_NAME_BOLD = "ExportedFontBold.bmp"
FILE_NAME_S = "ExportedFont8x8.bmp"
FILE_NAME_L = "ExportedFont.bmp"

OUTPUT_FILE_NAME_ENDIANESS_CONVERTED_L = "font.h"
OUTPUT_FILE_NAME_L = "font_non_arduino_hw.h"
OUTPUT_FILE_NAME_ENDIANESS_CONVERTED_S = "font8x8.h"
OUTPUT_FILE_NAME_S = "font8x8_non_arduino_hw.h"
CHAR_HEIGHT_L = 16
CHAR_WIDTH_L = CHAR_HEIGHT_L
CHAR_HEIGHT_S = 8
CHAR_WIDTH_S = CHAR_HEIGHT_S

def write_font_file(font_buffer, name, convert_endianess, char_width, char_height):
    f = open(name, 'w')
    header = """#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    #include <avr/pgmspace.h>
    #endif

    PROGMEM const """
    if(char_height == 16):
        header += "uint16_t font[]"
    elif (char_height == 8):
        header += "uint8_t font_small[]"
    else:
        raise Exception('char height', 'height of chars not supported')
    header += " = {\n"
    footer = "\n};"
    f.write(header)
    content = ""

    for letter in range(256):
        #sys.stdout.write("\n---------------------\n")
        content += "  // " + str(chr(letter+32)) + ":  " + str(letter) + "\n"
        for x in range(char_width):
            font_column = "  0b"
            part_a = ""
            part_b = ""
            for y in range(char_height):
                if(font_buffer[letter,x,y] == 1):
                    if (y < 8): part_a += "1"
                    if(y >= 8): part_b += "1"
                else:
                    if (y < 8): part_a += "0"
                    if(y >= 8): part_b += "0"
            if convert_endianess:
                font_column += part_b + part_a #because Arduino/AVR lib reads it big endian
            else:
                font_column += part_a + part_b
            content += font_column + ",\n"
        content += "\n\n"
    f.write(content)
    f.write(footer)
    f.close()

def get_header_val(f,name, pos, l):
    f.seek(pos)
    val = bytearray(f.read(l))
    if(l == 2):
        fmt = "h"
    elif(l == 4):
        fmt = "I"
    else:
        raise Exception('Format', 'no of bytes not supported')
    val = struct.unpack(fmt, val)[0] #get offset for image data
    print(name + ": " + str(val))
    return val

def process_bmp_file(file_name,char_width,char_height):
    size_bmp = os.path.getsize(file_name)
    print("Size of bmp: " + str(size_bmp))

    font_buffer = np.zeros((256,char_height,char_width)) # 3D array containing the font
    char_num = 0

    f = open(file_name,"rb")

    try:
            offset = get_header_val(f,"Offset", 10, 4)
            get_header_val(f,"biCompression", 30, 4)
            get_header_val(f,"biBitCount", 28, 2)
            get_header_val(f,"biClrUsed", 46, 4)
            width = get_header_val(f,"biWidth", 18, 4)
            height = get_header_val(f,"biHeight", 22, 4)

            f.seek(offset) #go to data beginning

            chars_per_line = int((width / char_width) - 1)

            reading_next_charset = False

            #linewise reading the image
            for line in reversed(range(height)):
                char_y = int(line % char_height) #calculate y position in current char
                if(char_y != char_height-1 and line != height-1):
                    char_num -= chars_per_line #now starting at beginning line again, so at the first char we looked at when we entered the column loop, -1 because of 0-indexing
                elif(line != height-1):
                    reading_next_charset = True

                f.seek(offset+line*width*3)
                for column in range(width):
                    char_x = int(column % char_width) #calculate x position in current char
                    if(char_x == 0 and column > 0):
                        char_num += 1 # check that we arent in the first column
                    elif(reading_next_charset):
                        char_num += 1
                        reading_next_charset = False

                    pixel = f.read(3) #read a pixel r,g,b bytes
                    #if(line > 238):
                    #    print("No:{},X:{},Y:{},line:{},column:{}".format(char_num,char_x, char_y, line, column))
                    if (pixel == b'\x00\x00\x00'):
                        font_buffer[char_num,char_x, char_y] = 0
                    else:
                        font_buffer[char_num,char_x, char_y] = 1


    finally:
        f.close()
    return font_buffer


write_font_file(process_bmp_file(FILE_NAME_L,CHAR_WIDTH_L,CHAR_HEIGHT_L),OUTPUT_FILE_NAME_ENDIANESS_CONVERTED_L,True,CHAR_WIDTH_L,CHAR_HEIGHT_L)
write_font_file(process_bmp_file(FILE_NAME_S,CHAR_WIDTH_S,CHAR_HEIGHT_S),OUTPUT_FILE_NAME_ENDIANESS_CONVERTED_S,True,CHAR_WIDTH_S,CHAR_HEIGHT_S)
write_font_file(process_bmp_file(FILE_NAME_L,CHAR_WIDTH_L,CHAR_HEIGHT_L),OUTPUT_FILE_NAME_L,False,CHAR_WIDTH_L,CHAR_HEIGHT_L)
write_font_file(process_bmp_file(FILE_NAME_S,CHAR_WIDTH_S,CHAR_HEIGHT_S),OUTPUT_FILE_NAME_S,False,CHAR_WIDTH_S,CHAR_HEIGHT_S)
