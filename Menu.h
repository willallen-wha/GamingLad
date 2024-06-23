int requestedGame;

void setupMenu();
void loopMenu();
boolean getInputMenu();
void drawMenu();

void setupMenu() {
  yPos = 1;
  requestedGame = 0;
}

void loopMenu() {
  if(requestedGame == 0) {
    drawMenu();
  }
}

boolean getInputMenu() {
  boolean updated = false;
  //Check for input from the board
  if(joystick.isUp()) {
    //Prevent overflow
    if(yPos < 4) yPos++;
    updated = true;
  } if(joystick.isDown()) {
    //Prevent overflow
    if(yPos > 1) yPos--;
    updated = true;
  }

  //Check to see if a flip is requested
  if(digitalRead(bluBut)) {
    if(yPos == 4) {
      showHighScores();
      drawMenu();
      return true;
    }
    requestedGame = yPos;
    updated = true;
  }

  return updated;
}

void drawMenu() {
  screen.fillRect(0, 0, 128, 160, TFT_BLACK);
  screen.setTextColor(TFT_WHITE, TFT_BLACK);
  screen.drawCentreString("GameBoi", 64, 10, 4);
  screen.drawCentreString("Please select", 64, 40, 2);
  screen.drawCentreString("a game.", 64, 55, 2);
  screen.drawCentreString("Minesweeper", 80, 80, 2);
  screen.drawCentreString("Tic-Tac-Toe", 80, 100, 2);
  screen.drawCentreString("Snake",       80, 120, 2);
  screen.drawCentreString("High Scores", 80, 140, 2);
  screen.drawLine(10, 60 + yPos * 20, 25, 70 + yPos * 20, TFT_BLUE);
  screen.drawLine(10, 80 + yPos * 20, 25, 70 + yPos * 20, TFT_BLUE);
  while(true) {
    if(getInputMenu()) {
      screen.fillRect(0, 75, 30, 85, TFT_BLACK);
      screen.drawLine(10, 60 + yPos * 20, 25, 70 + yPos * 20, TFT_BLUE);
      screen.drawLine(10, 80 + yPos * 20, 25, 70 + yPos * 20, TFT_BLUE);
      delay(DELAY);
    }
    if(requestedGame != 0) {
      break;
    }
  }
}
