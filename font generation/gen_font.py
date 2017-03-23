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

FILE_NAME= "ExportedFont.bmp"
OUTPUT_FILE_NAME = "font.h"
CHAR_HEIGHT = 16
CHAR_WIDTH = 16

def get_header_val(name, pos, l):
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

size_bmp = os.path.getsize(FILE_NAME)
print("Size of bmp: " + str(size_bmp))

font_buffer = np.zeros((256,16,16)) # 3D array containing the font
char_num = 0

f = open(FILE_NAME,"rb")

try:
        offset = get_header_val("Offset", 10, 4)
        get_header_val("biCompression", 30, 4)
        get_header_val("biBitCount", 28, 2)
        get_header_val("biClrUsed", 46, 4)
        width = get_header_val("biWidth", 18, 4)
        height = get_header_val("biHeight", 22, 4)

        f.seek(offset) #go to data beginning

        chars_per_line = int((width / CHAR_WIDTH) - 1)

        reading_next_charset = False

        #linewise reading the image
        for line in reversed(range(height)):
            char_y = int(line % CHAR_HEIGHT) #calculate y position in current char
            if(char_y != 15 and line != 255):
                char_num -= chars_per_line #now starting at beginning line again, so at the first char we looked at when we entered the column loop, -1 because of 0-indexing
            elif(line != 255):
                reading_next_charset = True

            f.seek(offset+line*width*3)
            for column in range(width):
                char_x = int(column % CHAR_WIDTH) #calculate x position in current char
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






f = open(OUTPUT_FILE_NAME, 'w')
header = "#include <avr/pgmspace.h>\n\nPROGMEM const uint16_t font[] = {\n"
footer = "\n};"
f.write(header)
content = ""

for letter in range(256):
    #sys.stdout.write("\n---------------------\n")
    content += "  // " + str(letter) + "\n"
    for x in range(16):
        font_column = "  0b"
        for y in range(CHAR_HEIGHT):
            if(font_buffer[letter,x,y] == 1):
                font_column += "1"
            else:
                font_column +=  "0"
        if(letter < 255):
            content += font_column + ",\n"
    content += "\n\n"
f.write(content)
f.write(footer)
f.close()