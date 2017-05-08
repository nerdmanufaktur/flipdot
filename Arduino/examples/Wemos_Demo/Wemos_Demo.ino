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
#include <ESP8266WiFi.h>
#include <FLIPDOT.h>
#include <ESPTime.h>
#include <Snake.h>

#define SSID "Mettigel24.de | Get off my LAN"
#define PASSWORD "N00bznet?NoWay!"
//change config in FLipdot.h if this changes!
#define BOARD_WIDTH 25
#define BOARD_HEIGHT 16
FLIPDOT board = FLIPDOT();
ESPTime timer = ESPTime();

/*
const int xPin = A0; //X attach to A0
const int yPin = D1; //Y attach to A1
const int btPin = D0; //Bt attach to digital 7
*/
Snake game = Snake(BOARD_WIDTH,BOARD_HEIGHT);

void setup() {
  Serial.begin(115200);
  WiFi.hostname("Flip-Dot");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  board.init(); //should come after wifi is connected on ESP8266
  board.render_string_small("My IP:");
  delay(1000);
  char ip[15];
  WiFi.localIP().toString().toCharArray(ip,15);
  board.scroll_string_small(ip);
  delay(1500);
}

enum {SHOW_SNAKE, SNAKE_INIT, SHOW_CLOCK, SHOW_SHUTTER};
int mode = SHOW_CLOCK;

void loop() {
  switch(mode)
  {game.start_game();
    case SNAKE_INIT:
        snake_start_animations();
        break;
    case SHOW_SNAKE:
        play_snake_game();
        break;
    case SHOW_CLOCK:
        show_time();
        break;
    case SHOW_SHUTTER:
        shutter_animation();
        break;
    default:;
  }
}

void show_time() {
  board.scroll_string(timer.getFormattedDate());
  delay(500);
  board.render_string_small("Oh it's");
  delay(1000);
  board.render_string(timer.getFormattedTime(), 4, FLIPDOT::ZERO_ALL);
  delay(1000);
  board.all_on();
  delay(200);
  board.all_off();
  for (float a = 0; a <= 2 * PI; a += 0.2) {
    int x = 1 + 8 * cos(a);
    int y = 4 + 8 * sin(a);
    board.render_string_small(timer.getFormattedTime(), x, y, FLIPDOT::ZERO_ALL);
  }
  for (float a = 2 * PI; a >= 0; a -= 0.2) {
    int x = 1 + 8 * cos(a);
    int y = 4 + 8 * sin(a);
    board.render_string_small(timer.getFormattedTime(), x, y, FLIPDOT::ZERO_ALL);
  }
float deg = 0;
  for (float a = 0; a <= BOARD_WIDTH; a++) {
    board.render_string_small(timer.getFormattedTime(), a,5.5+5.5*sin(deg), FLIPDOT::ZERO_NONE);
    deg += 0.2;
  }
  int len = strlen(timer.getFormattedTime()) * CHAR_OFFSET_SMALL * -1;
  for (float a = BOARD_WIDTH; a >= len; a-=1) {
    board.render_string_small(timer.getFormattedTime(), a, 5.5+5.5*sin(deg));
    deg -= 0.2;
  }
 
  mode = SHOW_SHUTTER;
}

void play_snake_game(){
  if(game.step(Snake::AUTO)){
    //game.render_frame_buffer();
    board.reset_internal_framebuffer();
    for(unsigned int x = 0; x < BOARD_WIDTH; x++){
      for(unsigned int y = 0; y < BOARD_HEIGHT; y++){
        if(game.pixel_is_set(x,y)){
          board.draw_in_internal_framebuffer(1,(uint8_t)x,(uint8_t)y);
        }
      }
    }
    board.render_internal_framebuffer();
  } else {
    snake_show_score();
  }
}

void snake_show_score(){
    board.render_string_small("Game",3,1, FLIPDOT::ZERO_ALL);
    board.render_string_small("Over!",1,8, FLIPDOT::ZERO_NONE);
    delay(1500);
    char highscore[5];
    itoa(game.score, highscore, 10);
    board.render_string_small("Score:",0,0, FLIPDOT::ZERO_ALL);
    delay(1000);
    board.render_string_small(highscore,8,8, FLIPDOT::ZERO_NONE);

    mode = SHOW_CLOCK;
}

void snake_start_animations() {
  board.render_string_small("SNAKE",0,4, FLIPDOT::ZERO_ALL);
    delay(1000);
    board.scroll_string("ON FLIPDOT");
    board.all_on();
    delay(300);
    board.all_off();
    delay(300);
    board.all_on();
    delay(100);
    board.all_off();
    board.render_string_small("Let's",0,4, FLIPDOT::ZERO_ALL);
    delay(700);
    board.all_on();
    delay(100);
    board.all_off();
    board.render_string("GO", 3);
    delay(600);
    board.all_on();
    delay(150);
    board.all_off();
    delay(500);
    game.start_game();

    mode = SHOW_SNAKE;
}

void shutter_animation() {
  for (int i = 100; i > 0; i -= 5) {
    board.all_on();
    delay(i);
    board.all_off();
    delay(i);
  }

  mode = SNAKE_INIT;
}

