#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <AxisJoystick.h> // Useful joystick library
#include "RTClib.h" //Library for the RTC
#include <Wire.h>

// Define some pins
#define RTC_SDA 16
#define RTC_SCL 17
#define xIn 25
#define yIn 26
#define joyBut 27
#define bluBut 34
#define greBut 35

// Define some values
#define upperBound 3000
#define DELAY 250

#define TFT_GREY 0x7BEF
#define TFT_W 160
#define TFT_H 128

//Define the joystick
AxisJoystick joystick(joyBut, xIn, yIn);
//Define the screen
TFT_eSPI screen = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
//Create RTC Object
TwoWire I2CRTC = TwoWire(0);
RTC_DS3231 rtc;
long getTime();

//Keep x and y variables
int xPos = 2, yPos = 2;

//Keep track of the game being played
#define MAINMENU 0
#define MINESWEEPER 1
#define TICTACTOE 2
#define SNAKE 3
int currentGame;
boolean paused;
boolean inPause;
//The highest scores of the games
long highScoreMSEasy;
long highScoreMSHard;
long pauseTime;
int highScoreTTTOne, highScoreTTTTwo; //Likely just for legacy
int highScoreSnake;
void showHighScores();

#include "Minesweeper.h"
#include "Menu.h"
#include "Tic-Tac-Toe.h"
#include "Snake.h"

//Pause the game - Interrupt Service Routine
void IRAM_ATTR ISR();

void setup() {
  
  //Calibrate the joystick with expected bounds
  joystick.calibrate(500, 2500);

  //Initialize the RTC
  //Initialize communication on SDA, SCL, and clock frequence of 40 MHz
  I2CRTC.begin(RTC_SDA, RTC_SCL, 40000000);
  rtc.begin(&I2CRTC);
  rtc.adjust(DateTime(__DATE__, __TIME__));
  
  //Calibrate random ints
  randomSeed(analogRead(A0));
  
  // Setup the LCD
  screen.init();
  screen.setRotation(2);

  //Set the buttons as input
  pinMode(bluBut, INPUT);
  pinMode(greBut, INPUT);

  //Initialize the screen
  screen.fillScreen(TFT_BLACK);

  Serial.begin(9600);

  //Set up the pause button
  attachInterrupt(greBut, ISR, FALLING);
  
  currentGame = MAINMENU;
  highScoreMSEasy = -1;  //There isn't an Int.MAX_VALUE define which is very frustrating to me
  highScoreMSHard = -1;  //There isn't an Int.MAX_VALUE define which is very frustrating to me
  highScoreTTTOne = 0; highScoreTTTTwo = 0; //This is here for completion, realistically it won't be included
  highScoreSnake = 0;
  setupMenu();
  paused = false;
  pauseTime = 0;
  drawMenu();
  
}

void loop() {
  if(currentGame == MAINMENU) {
    paused = false;
    if(requestedGame != 0) {
      switchGame(requestedGame);
      return;
    }
    loopMenu();
  }
  else if(paused) {
    drawPause();
  }
  else if(currentGame == MINESWEEPER) {
    if(!playingMinesweeper) {
      switchGame(MAINMENU);
      return;
    }
    loopMS();
  }
  else if(currentGame == TICTACTOE) {
    if(!playingTTT) {
      switchGame(MAINMENU);
      return;
    }
    loopTTT();
  }
  else if(currentGame == SNAKE) {
    if(!playingSnake) {
      switchGame(MAINMENU);
      return;
    }
    loopSnake();
  }
}

void drawPause() {
  //Start the pause timer
  long startPause = getTime();
  //Pause the game
  inPause = true;
  screen.fillRect(0, 38, 128, 64, TFT_GREY);
  screen.fillRect(2, 40, 124, 60, TFT_BLACK);
  screen.setTextColor(TFT_WHITE, TFT_BLACK);
  screen.drawCentreString("PAUSE", 64, 42, 4);
  screen.drawCentreString("Continue", 64, 70, 2);
  screen.drawCentreString("Quit", 64, 85, 2);
  int choice = 0;
  boolean changed;
  screen.drawLine(10, choice * 15 + 72, 15, choice * 15 + 77, TFT_BLUE);
  screen.drawLine(10, choice * 15 + 82, 15, choice * 15 + 77, TFT_BLUE);
  while(true) {
    changed = false;
    if(joystick.isDown() && choice > 0) {
      choice--;
      changed = true;
    }
    if(joystick.isUp() && choice < 1) {
      choice++;
      changed = true;
    }
    if(changed) {
      screen.fillRect(2, 70, 20, 30, TFT_BLACK);
      screen.drawLine(10, choice * 15 + 72, 15, choice * 15 + 77, TFT_BLUE);
      screen.drawLine(10, choice * 15 + 82, 15, choice * 15 + 77, TFT_BLUE);
      delay(DELAY);
    }
    if(digitalRead(greBut)) while(digitalRead(greBut)) {paused = false;}
    //If an option is selected
    if(digitalRead(bluBut)) {
      //Continue selected
      if(choice == 0) paused = false;
      else {
        //Quit was selected
        //Un-mark the current game as being played
        playingMinesweeper = false;
        playingTTT = false;
        playingSnake = false;
        switchGame(MAINMENU);
        paused = false;
      }
      //Wait for hanging inputs to go away
      while(digitalRead(bluBut));
    }
    if(!paused) break;
  }
  //Update so the pause menu goes away ASAP
  if(currentGame == MAINMENU) drawMenu();
  if(currentGame == MINESWEEPER) drawGridMS();
  if(currentGame == TICTACTOE) drawGridTTT();
  if(currentGame == SNAKE) drawGridSnake();
  inPause = false;
  //Now that the pause is done, update accordingly
  long endPause = getTime();
  pauseTime += (endPause - startPause);
}

void switchGame(int requested) {
  if(requested == MAINMENU) {
    currentGame = MAINMENU;
    setupMenu();
  }
  else if(requested == MINESWEEPER) {
    currentGame = MINESWEEPER;
    setupMS();
  }
  else if(requested == TICTACTOE) {
    currentGame = TICTACTOE;
    setupTTT();
  }
  else if(requested == SNAKE) {
    currentGame = SNAKE;
    setupSnake();
  }
  else {
    currentGame = MAINMENU;
    setupMenu();
  }
  //Wait for any lingering presses to be released
  while(digitalRead(bluBut));
}


long getTime() {
  long result = 0;
  DateTime current = rtc.now();
  result += (long) current.second();
  result += (long) (current.minute() * 60);
  result += (long) ((current.hour() * 60) * 60);
  return result;
}

void showHighScores() {
  //Black out the screen
  screen.fillRect(0, 0, 160, 160, TFT_BLACK);
  screen.setTextColor(TFT_WHITE, TFT_BLACK);
  screen.drawCentreString("HIGH SCORES", 64, 0, 2);
  screen.drawCentreString("Minesweeper, Easy:", 64, 32, 2);
  screen.drawCentreString("Minesweeper, Hard:", 64, 64, 2);
  screen.drawCentreString("Snake:", 64, 96, 2);
  //Check each score to make sure it's set
  if(highScoreMSEasy != -1) screen.drawCentreString(String(highScoreMSEasy / (60 * 60)) + ":" + String((highScoreMSEasy % (60 * 60)) / 60) + ":" + String(highScoreMSEasy % 60), 64, 48, 2);
  else screen.drawCentreString("N/A", 64, 48, 2);
  if(highScoreMSHard != -1) screen.drawCentreString(String(highScoreMSHard / (60 * 60)) + ":" + String((highScoreMSHard % (60 * 60)) / 60) + ":" + String(highScoreMSHard % 60), 64, 80, 2);
  else screen.drawCentreString("N/A", 64, 80, 2);
  if(highScoreSnake != 0) screen.drawCentreString(String(highScoreSnake), 64, 112, 2);
  else screen.drawCentreString("N/A", 64, 112, 2);
  //Draw the 'exit button'
  screen.fillRect(28, 130, 72, 25, TFT_GREY);
  screen.fillRect(30, 132, 68, 21, TFT_YELLOW);
  screen.setTextColor(TFT_BLACK, TFT_YELLOW);
  screen.drawCentreString("CONTINUE", 64, 134, 2);
  delay(DELAY);
  //Wait for button press, then wait for button unpress
  while(!digitalRead(bluBut));
  while(digitalRead(bluBut));
}

void IRAM_ATTR ISR() {
  if(inPause) return;
  paused = true;
  needsRepainted = true;
}
