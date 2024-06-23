//Keep the minesweeper grid
int** gridTTT;

//Variable for outer use
boolean playingTTT;

//Keep track of whose turn it is
boolean playerOneTurn;

//Keep track of wins
int playerOneWins, playerTwoWins;

//Declare the functions
void setupTTT();
void loopTTT();
boolean getInputTTT();
void remakeGridTTT();
void drawGridTTT();
void requestPlaceTTT();
void checkForWinTTT();
void gameOverTTT(boolean winner);
void gameOverTTT();

void setupTTT()
{

  playingTTT = true;

  remakeGridTTT();
  playerOneTurn = true;
  playerOneWins = 0; playerTwoWins = 0;

  screen.fillRect(0, 0, 160, 160, TFT_ORANGE);
  drawGridTTT();
}

void loopTTT()
{

  if(getInputTTT()) {
    drawGridTTT();
    checkForWinTTT();
    delay(DELAY);
  }

}

//Get input from the user
boolean getInputTTT() {
  boolean updated = false;
  //Check for input from the grid
  if(joystick.isUp()) {
    //Prevent overflow
    if(yPos < 2) yPos++;
    updated = true;
  } if(joystick.isDown()) {
    //Prevent overflow
    if(yPos > 0) yPos--;
    updated = true;
  }
  if(joystick.isRight()) {
    //Prevent overflow
    if(xPos < 2) xPos++;
    updated = true;
  } if(joystick.isLeft()) {
    //Prevent overflow
    if(xPos > 0) xPos--;
    updated = true;
  }

  //Check to see if a move is requested
  if(digitalRead(bluBut)) {
    requestPlaceTTT();
    updated = true;
    //Wait for lingering presses to go away
    while(digitalRead(bluBut));
  }

  return updated;
}

//Create a grid of the desired size
void remakeGridTTT() {
  xPos = 1; yPos = 1;
  gridTTT = new int*[3];
  for(int x = 0; x < 3; x++) {
    gridTTT[x] = new int[3];
    for(int y = 0; y < 3; y++) {
      gridTTT[x][y] = 0;
    }
  }
}

//Draw the grid in its current state
void drawGridTTT() {
  //Draw the top UI
  screen.setCursor(2, 2);
  //Make the current player's name red
  if(playerOneTurn) screen.setTextColor(TFT_RED, TFT_ORANGE);
  else screen.setTextColor(TFT_BLACK, TFT_ORANGE);
  screen.print("Player 1");
  screen.setCursor(78, 2);
  if(playerOneTurn) screen.setTextColor(TFT_BLACK, TFT_ORANGE);
  else screen.setTextColor(TFT_RED, TFT_ORANGE);
  screen.print("Player 2");

  //Draw the running scores
  screen.setTextColor(TFT_BLACK, TFT_ORANGE);
  screen.drawCentreString(String(playerOneWins) + ":" + String(playerTwoWins), 64, 10, 4);
  
  //Draw the grid
  //Clear the previous grid
  screen.fillRect(0, 32, 128, 128, TFT_ORANGE);
  //Make each line 2 thick
  for(int i = 0; i < 2; i++) {
    //Vertical
    screen.drawLine(44 + i, 37, 44 + i, 155, TFT_BLACK);
    screen.drawLine(84 + i, 37, 84 + i, 155, TFT_BLACK);
    //Horizontal
    screen.drawLine(5, 76 + i, 123, 76 + i, TFT_BLACK);
    screen.drawLine(5, 116 + i, 123, 116 + i, TFT_BLACK);
  }

  //Fill in the grid
  for(int x = 0; x < 3; x++) {
    for(int y = 0; y < 3; y++) {
      //Draw the appropriate thing at that spot
      if(gridTTT[x][y] == 1) {
        //Three thick lines for X
        for(int i = 0; i < 3; i++) {
          screen.drawLine(10 + 39 * x + i, 42 + 39 * y, 39 + 39 * x + i, 71 + 39 * y, TFT_BLACK);
          screen.drawLine(39 + 39 * x + i, 42 + 39 * y, 10 + 39 * x + i, 71 + 39 * y, TFT_BLACK);
        }
      }
      //Three thick circle for O
      else if(gridTTT[x][y] == -1) {
        for(int i = 0; i < 5; i ++) {
          screen.drawCircle(24 + 40 * x, 56 + 40 * y, (45 + i) / 3, TFT_BLACK);
        }
      }
    }
  }

  //Mark the current player's position
  //Plus for the X player, circle for the O player
  if(playerOneTurn) {
      screen.drawLine(15 + 40 * xPos, 57 + 39 * yPos, 35 + 40 * xPos, 57 + 39 * yPos, TFT_GREY);
      screen.drawLine(25 + 40 * xPos, 47 + 39 * yPos, 25 + 40 * xPos, 67 + 39 * yPos, TFT_GREY);
  } else {
      screen.drawCircle(24 + 40 * xPos, 56 + 40 * yPos, 10, TFT_GREY);
  }
}

void requestPlaceTTT() {
  //Make sure that spot isn't taken yet
  if(gridTTT[xPos][yPos] == 0) {
    //Check whose turn it is
    if(playerOneTurn) gridTTT[xPos][yPos] =  1;
    else              gridTTT[xPos][yPos] = -1;
    playerOneTurn = !playerOneTurn;
  }
}

void checkForWinTTT() {
  //Check all 8 possible win conditions
  for(int i = 0; i < 3; i++) {
    //Vertical wins
    if(abs(gridTTT[i][0] + gridTTT[i][1] + gridTTT[i][2]) == 3) {
      //Draw a line showing victory
      for(int j = 0; j < 3; j++) {
        screen.drawLine(23 + 39 * i + j, 42, 23 + 39 * i + j, 150, TFT_RED);
      }
      gameOverTTT(gridTTT[i][0] == 1);
    }
    //Horizontal wins
    if(abs(gridTTT[0][i] + gridTTT[1][i] + gridTTT[2][i]) == 3) {
      for(int j = 0; j < 3; j++) {
        screen.drawLine(10, 56 + 39 * i + j, 118, 56 + 39 * i + j, TFT_RED);
      }
      gameOverTTT(gridTTT[0][i] == 1);
    }
  }
  //Angular wins
  if(abs(gridTTT[0][0] + gridTTT[1][1] + gridTTT[2][2]) == 3) {
    for(int j = 0; j < 3; j++) {
      screen.drawLine(9 + j, 42, 117 + j, 150, TFT_RED);
    }
    gameOverTTT(gridTTT[0][0] == 1);
  }
  if(abs(gridTTT[0][2] + gridTTT[1][1] + gridTTT[2][0]) == 3) {
    for(int j = 0; j < 3; j++) {
      screen.drawLine(117 + j, 42, 9 + j, 150, TFT_RED);
    }
    gameOverTTT(gridTTT[0][2] == 1);
  }

  //Did no one win?
  boolean canPlay = false;
  for(int x = 0; x < 3; x++) {
    for(int y = 0; y < 3; y++) {
      if(gridTTT[x][y] == 0) {
        canPlay = true;
        x = 5; y = 5;
        break;
      }
    }
  }
  if(!canPlay) gameOverTTT();
}

void gameOverTTT(boolean playerOneVictory) {
  //Increment the wins of the person who won
  if(playerOneVictory) playerOneWins++;
  else playerTwoWins++;
  //Give em a second to look at it
  delay(DELAY); delay(DELAY);
  
  //I stole this code from minesweeper
  screen.fillRect(0, 40, 128, 80, TFT_BLACK);
  screen.setTextColor(TFT_WHITE, TFT_BLACK);
  screen.drawCentreString("Player 1: " + String(playerOneWins), 64, 62, 1);
  screen.drawCentreString("Player 2: " + String(playerTwoWins), 64, 75, 1);
  screen.drawCentreString("Rematch", 64, 85, 2);
  screen.drawCentreString("Quit", 64, 100, 2);
  if(playerOneVictory) {
    screen.drawCentreString("Player 1 Wins!", 64, 42, 2);
  } else {
    screen.drawCentreString("Player 2 Wins!", 64, 42, 2);
  }
  yPos = 0;
  boolean changed;
  screen.drawLine(10, yPos * 15 + 87, 15, yPos * 15 + 92, TFT_BLUE);
  screen.drawLine(10, yPos * 15 + 97, 15, yPos * 15 + 92, TFT_BLUE);
  while(true) {
    changed = false;
    if(joystick.isDown() && yPos > 0) {
      yPos--;
      changed = true;
    }
    if(joystick.isUp() && yPos < 1) {
      yPos++;
      changed = true;
    }
    if(changed) {
      screen.fillRect(0, 70, 20, 45, TFT_BLACK);
      screen.drawLine(10, yPos * 15 + 87, 15, yPos * 15 + 92, TFT_BLUE);
      screen.drawLine(10, yPos * 15 + 97, 15, yPos * 15 + 92, TFT_BLUE);
      delay(DELAY);
    }
    //If an option is selected
    if(digitalRead(bluBut)) {
      //Easy mode selected
      if(yPos == 0)  {
        remakeGridTTT();
        drawGridTTT();
      }
      else {
        playingTTT = false;
      }
      while(digitalRead(bluBut));
      break;
    }
  }
}

//Special version where it's a tie
void gameOverTTT() {
  delay(DELAY); delay(DELAY);
  
  //I stole this code from minesweeper
  screen.fillRect(0, 40, 128, 80, TFT_BLACK);
  screen.setTextColor(TFT_WHITE, TFT_BLACK);
  screen.drawCentreString("It's a tie!", 64, 42, 2);
  screen.drawCentreString("Player 1: " + String(playerOneWins), 64, 62, 1);
  screen.drawCentreString("Player 2: " + String(playerTwoWins), 64, 75, 1);
  screen.drawCentreString("Rematch", 64, 85, 2);
  screen.drawCentreString("Quit", 64, 100, 2);
  
  yPos = 0;
  boolean changed;
  screen.drawLine(10, yPos * 15 + 87, 15, yPos * 15 + 92, TFT_BLUE);
  screen.drawLine(10, yPos * 15 + 97, 15, yPos * 15 + 92, TFT_BLUE);
  while(true) {
    changed = false;
    if(joystick.isDown() && yPos > 0) {
      yPos--;
      changed = true;
    }
    if(joystick.isUp() && yPos < 1) {
      yPos++;
      changed = true;
    }
    if(changed) {
      screen.fillRect(0, 70, 20, 45, TFT_BLACK);
      screen.drawLine(10, yPos * 15 + 87, 15, yPos * 15 + 92, TFT_BLUE);
      screen.drawLine(10, yPos * 15 + 97, 15, yPos * 15 + 92, TFT_BLUE);
      delay(DELAY);
    }
    //If an option is selected
    if(digitalRead(bluBut)) {
      //Easy mode selected
      if(yPos == 0)  {
        remakeGridTTT();
        drawGridTTT();
      }
      else {
        playingTTT = false;
      }
      while(digitalRead(bluBut));
      break;
    }
  }
}
