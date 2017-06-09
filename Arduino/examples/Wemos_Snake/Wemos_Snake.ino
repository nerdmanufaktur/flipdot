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
#define BOARD_WIDTH 115
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
  game.start_game();
  snake_start_animations();
}

void loop() {
  /*
  int x = analogRead(xPin);
  int y = digitalRead(yPin);
  int bt = digitalRead(btPin);
  if(x < 400){
    dir = SNAKE_LEFT;
  } else if (x > 900) {
    dir = SNAKE_RIGHT;
  } else if (y == 0) {
    dir = SNAKE_UP;
  } else if (bt == 0) {
    dir = SNAKE_DOWN;
  }
  */
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
    delay(1500);
    game.start_game();
    snake_start_animations();
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
}

