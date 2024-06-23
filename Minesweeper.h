//Keep the minesweeper grid
int** gridMS;
int gridWidth = 8;
int gridHeight = 10;
int squareSize = 32;

//Variable for outer use
boolean playingMinesweeper;

//Variable to hold the start time
long startTime;
boolean newHighScoreMS;

//Declare the functions
void setupMS();
void loopMS();
boolean getInputMS();
void makeGridMS(int desiredW, int desiredH);
void drawGridMS();
void requestFlipMS();
void markBombMS();
void checkForWinMS();
void gameOverMS(boolean victory);

void setupMS()
{

  playingMinesweeper = true;

  //Get the desired difficulty
  screen.fillRect(0, 38, 128, 84, TFT_GREY);
  screen.fillRect(2, 40, 124, 80, TFT_BLACK);
  screen.setTextColor(TFT_WHITE, TFT_BLACK);
  screen.drawCentreString("Easy mode", 64, 70, 2);
  screen.drawCentreString("Hard mode", 64, 85, 2);
  screen.drawCentreString("Quit", 64, 100, 2);
  screen.drawCentreString("Select difficulty", 64, 42, 2);
  yPos = 0;
  boolean changed;
  screen.drawLine(10, yPos * 15 + 72, 15, yPos * 15 + 77, TFT_BLUE);
  screen.drawLine(10, yPos * 15 + 82, 15, yPos * 15 + 77, TFT_BLUE);
  while(true) {
    changed = false;
    if(joystick.isDown() && yPos > 0) {
      yPos--;
      changed = true;
    }
    if(joystick.isUp() && yPos < 2) {
      yPos++;
      changed = true;
    }
    if(changed) {
      screen.fillRect(2, 70, 20, 45, TFT_BLACK);
      screen.drawLine(10, yPos * 15 + 72, 15, yPos * 15 + 77, TFT_BLUE);
      screen.drawLine(10, yPos * 15 + 82, 15, yPos * 15 + 77, TFT_BLUE);
      delay(DELAY);
    }
    //If an option is selected
    if(digitalRead(bluBut)) {
      //Easy mode selected
      if(yPos == 0) makeGridMS(4, 5);
      else if(yPos == 1) makeGridMS(8, 10);
      else {
        playingMinesweeper = false;
        return;
      }
      while(digitalRead(bluBut));
      drawGridMS();
      break;
    }
  }

  drawGridMS();
  
}

void loopMS()
{

  if(getInputMS()) {
    drawGridMS();
    checkForWinMS();
    delay(DELAY);
  }

}

//Get input from the user
boolean getInputMS() {
  boolean updated = false;
  //Check for input from the board
  if(joystick.isUp()) {
    //Prevent overflow
    if(yPos < gridHeight - 1) yPos++;
    updated = true;
  } if(joystick.isDown()) {
    //Prevent overflow
    if(yPos > 0) yPos--;
    updated = true;
  }
  if(joystick.isRight()) {
    //Prevent overflow
    if(xPos < gridWidth - 1) xPos++;
    updated = true;
  } if(joystick.isLeft()) {
    //Prevent overflow
    if(xPos > 0) xPos--;
    updated = true;
  }

  //Check to see if a flip is requested
  if(digitalRead(bluBut)) {
    requestFlipMS();
    updated = true;
  }

  //Check to see if marking as a bomb is requested
  if(joystick.isPress()){
    markBombMS();
    updated = true;
  }

  return updated;
}

//Create a grid of the desired size
void makeGridMS(int desiredW, int desiredH) {
  xPos = 2; yPos = 2;
  gridWidth = desiredW; gridHeight = desiredH;
  gridMS = new int*[gridWidth];
  for(int x = 0; x < gridWidth; x++) {
    gridMS[x] = new int[gridHeight];
    for(int y = 0; y < gridHeight; y++) {
      gridMS[x][y] = -100;
    }
  }
  squareSize = 128 / desiredW;
}

//Draw the grid in its current state
void drawGridMS() {
  for(int x = 0; x < gridWidth; x++) {
    for(int y = 0; y < gridHeight; y++) {

      //Useful variables
      int innerX = x * squareSize + 2; int innerY = y * squareSize + 2;
      int outerX = x * squareSize; int outerY = y * squareSize;
      
      //If this is the selected spot show accordingly
      if(x == xPos && y == yPos)screen.fillRect(outerX, outerY, squareSize, squareSize, TFT_BLUE);
      //Otherwise don't have that one selected
      else screen.fillRect(outerX, outerY, squareSize, squareSize, TFT_BLACK);
      //Draw the cube
      screen.fillRect(innerX, innerY, squareSize - 4, squareSize - 4, TFT_GREY);

      //Set up the text
      screen.setTextWrap(true, true);
      screen.setCursor(innerX, innerY);
      screen.setTextColor(TFT_WHITE, TFT_GREY);
      
      //If the int is negative, it's obscured
      if(gridMS[x][y] < 0) {
        //If the int is lower than negative 10, it has been marked
        if(gridMS[x][y] < -10 && gridMS[x][y] != -100) screen.drawCentreString("?", (2 * innerX + squareSize - 4) / 2, innerY + 4, 8 / gridWidth);
        //Otherwise, it's just an unknown
        //Don't print anything
      }
      //If the int is positive but less than 10, then it has some value we care about
      else if(gridMS[x][y] < 10) screen.drawCentreString(String(gridMS[x][y]), (2 * innerX + squareSize - 4) / 2, innerY + 4, 8 / gridWidth);
      //If the int is 10, then a bomb has been flipped and the game is lost
      else if(gridMS[x][y] == 10) screen.drawCentreString("X", (2 * innerX + squareSize - 4) / 2, innerY + 4, 8 / gridWidth);
    }
  }
}

void requestFlipMS() {
  //Check to see if any have been flipped yet
  if(gridMS[0][0] != -100) {
    //Check to make sure that spot hasn't been marked
    if(gridMS[xPos][yPos] < -10) return;
    //Flip that item upside down, or make it positive if it's not already
    gridMS[xPos][yPos] = abs(gridMS[xPos][yPos]);
  }
  else {
    //New game has been requested
    //Make bombs fill 1/3 of the total spots
    int spots = gridWidth * gridHeight / 3;
    while(spots > 0) {
      int randX = random(gridWidth);
      int randY = random(gridHeight);
      //Make sure not hitting within one of the stating square
      if(randX < xPos - 1 || randX > xPos + 1) {
        if(gridMS[randX][randY] != -10) {
          gridMS[randX][randY] = -10;
          spots--;
        }
      }
      if(randY < yPos - 1 || randY > yPos + 1) {
        if(gridMS[randX][randY] != -10) {
          gridMS[randX][randY] = -10;
          spots--;
        }
      }
    }
    //Now that all the bombs have been placed, make everything else reflect how many spots are around them
    for(int x = 0; x < gridWidth; x++) {
      for(int y = 0; y < gridHeight; y++) {
        //Make sure we're not updating a bomb
        if(gridMS[x][y] != -10) {
          gridMS[x][y] = 0;
          //Check the eight around them when applicable
          //The 4 cardinals
          if(x - 1 >= 0         && gridMS[x - 1][y] == -10) gridMS[x][y]--;
          if(y - 1 >= 0         && gridMS[x][y - 1] == -10) gridMS[x][y]--;
          if(x + 1 < gridWidth  && gridMS[x + 1][y] == -10) gridMS[x][y]--;
          if(y + 1 < gridHeight && gridMS[x][y + 1] == -10) gridMS[x][y]--;
          //The 4 diagonals
          if(x - 1 >= 0        && y - 1 >= 0         && gridMS[x - 1][y - 1] == -10) gridMS[x][y]--;
          if(x - 1 >= 0        && y + 1 < gridHeight && gridMS[x - 1][y + 1] == -10) gridMS[x][y]--;
          if(x + 1 < gridWidth && y - 1 >= 0         && gridMS[x + 1][y - 1] == -10) gridMS[x][y]--;
          if(x + 1 < gridWidth && y + 1 < gridHeight && gridMS[x + 1][y + 1] == -10) gridMS[x][y]--;
        }
      }
    }
    //Now make things that should be visible visible
    for(int x = 0; x < gridWidth; x++) {
      for(int y = 0; y < gridHeight; y++) {
        //Check the eight around them when applicable
        //The 4 cardinals
        if(x - 1 >= 0         && gridMS[x - 1][y] == 0) gridMS[x][y] = abs(gridMS[x][y]);
        if(y - 1 >= 0         && gridMS[x][y - 1] == 0) gridMS[x][y] = abs(gridMS[x][y]);
        if(x + 1 < gridWidth  && gridMS[x + 1][y] == 0) gridMS[x][y] = abs(gridMS[x][y]);
        if(y + 1 < gridHeight && gridMS[x][y + 1] == 0) gridMS[x][y] = abs(gridMS[x][y]);
        //The 4 diagonals
        if(x - 1 >= 0         && y - 1 >= 0         && gridMS[x - 1][y - 1] == 0) gridMS[x][y] = abs(gridMS[x][y]);
        if(x - 1 >= 0         && y + 1 < gridHeight && gridMS[x - 1][y + 1] == 0) gridMS[x][y] = abs(gridMS[x][y]);
        if(x + 1 < gridWidth  && y - 1 >= 0         && gridMS[x + 1][y - 1] == 0) gridMS[x][y] = abs(gridMS[x][y]);
        if(x + 1 < gridWidth  && y + 1 < gridHeight && gridMS[x + 1][y + 1] == 0) gridMS[x][y] = abs(gridMS[x][y]);
      }
    }
    //The game is afoot, set the start time
    startTime = getTime();
    pauseTime = 0;
    newHighScoreMS = false;
  }
}

void markBombMS() {
  //Make sure the game has started
  if(gridMS[xPos][yPos] == -100) return;
  //Check to see if it has been marked already
  if(gridMS[xPos][yPos] < -10) gridMS[xPos][yPos] += 10;
  //If not, make sure it's not visible
  else if(gridMS[xPos][yPos] >= 0) return;
  else gridMS[xPos][yPos] -= 10;
}

void checkForWinMS() {
  boolean lost = false;
  int bombs = 0, marked = 0, remainingUnknown = 0;
  for(int x = 0; x < gridWidth; x++) {
    for(int y = 0; y < gridHeight; y++) {
      //If there's an overturned bomb, lose
      if(gridMS[x][y] == 10) {
        lost = true;
        x = gridWidth;
        y = gridHeight;
        break;
      }
      //Count how many unfound bombs there are
      else if(gridMS[x][y] == -10) {
        bombs++;
      }
      else if(gridMS[x][y] == -20) {
        marked++;
        bombs++;
      }
      else if(gridMS[x][y] < -10) {
        marked++;
      }
      //Count how many things there are that we don't know
      if(gridMS[x][y] < 0) {
        remainingUnknown++;
      }
    }
  }
  //If we've lost we've lost
  if(lost) gameOverMS(false);
  //If the only things we haven't seen are bombs we are done
  else if(bombs == remainingUnknown) {
    gameOverMS(true);
  } else if(gridMS[0][0] != -100){
    if(yPos != 0) {
      screen.fillRect(8, 0, 112, 10, TFT_BLUE);
      screen.drawCentreString("Bombs remaining: " + String(bombs - marked), 64, 1, 1);
    }
    else {
      screen.fillRect(8, 150, 112, 10, TFT_BLUE);
      screen.drawCentreString("Bombs remaining: " + String(bombs - marked), 64, 151, 1);
    }
  }
}

void gameOverMS(boolean victory) {
  long totalTime;
  //If it's a win, get the time for the win
  if(victory) {
    long endTime = getTime();
    totalTime = endTime - startTime;
    //Adjust for time spent paused
    totalTime -= pauseTime;
    //If there isn't a score yet or if our time is lower
    //If it's easy mode?
    if(gridWidth = 4) {
      if(highScoreMSEasy == -1 || totalTime < highScoreMSEasy) {
        newHighScoreMS = true;
        highScoreMSEasy = totalTime;
      }
    }
    else {
      if(highScoreMSHard == -1 || totalTime < highScoreMSHard) {
        newHighScoreMS = true;
        highScoreMSHard = totalTime;
      }
    }
  }
  //Reveal the maze to them so they can see they goofed it if they lost
  if(!victory) {
    for(int x = 0; x < gridWidth; x++) {
      for(int y = 0; y < gridHeight; y++) {
        //Unmark marked spots
        if(gridMS[x][y] < -10) gridMS[x][y] += 10;
        gridMS[x][y] = abs(gridMS[x][y]);
      }
    }
    drawGridMS();
    //Hold until a button is pressed
    while(!digitalRead(bluBut));
    //Hold until that button is released
    while(digitalRead(bluBut));
  }
  delay(DELAY);
  delay(DELAY);
  //No matter what, we want the menu to pop up and get input
  screen.fillRect(0, 40, 128, 80, TFT_BLACK);
  screen.setTextColor(TFT_WHITE, TFT_BLACK);
  screen.drawCentreString("Easy mode", 64, 70, 2);
  screen.drawCentreString("Hard mode", 64, 85, 2);
  screen.drawCentreString("Quit", 64, 100, 2);
  screen.drawCentreString("Play again?", 64, 55, 2);
  if(newHighScoreMS) screen.drawCentreString("New high score!", 64, 55, 2);
  if(victory) {
    screen.drawCentreString("You won! Time: " + String(totalTime / (60 * 60)) + ":" + String((totalTime % (60 * 60)) / 60) + ":" + String(totalTime % 60), 64, 42, 1);
  } else {
    screen.drawCentreString("You lost. :(", 64, 42, 2);
  }
  yPos = 0;
  boolean changed;
  screen.drawLine(10, yPos * 15 + 72, 15, yPos * 15 + 77, TFT_BLUE);
  screen.drawLine(10, yPos * 15 + 82, 15, yPos * 15 + 77, TFT_BLUE);
  while(true) {
    changed = false;
    if(joystick.isDown() && yPos > 0) {
      yPos--;
      changed = true;
    }
    if(joystick.isUp() && yPos < 2) {
      yPos++;
      changed = true;
    }
    if(changed) {
      screen.fillRect(0, 70, 20, 45, TFT_BLACK);
      screen.drawLine(10, yPos * 15 + 72, 15, yPos * 15 + 77, TFT_BLUE);
      screen.drawLine(10, yPos * 15 + 82, 15, yPos * 15 + 77, TFT_BLUE);
      delay(DELAY);
    }
    //If an option is selected
    if(digitalRead(bluBut)) {
      //Easy mode selected
      if(yPos == 0) makeGridMS(4, 5);
      else if(yPos == 1) makeGridMS(8, 10);
      else playingMinesweeper = false;
      while(digitalRead(bluBut));
      drawGridMS();
      break;
    }
  }
}
