//Linked-list style implementation of snake segments
struct segment {
  private:
    //X and Y coordinate of that segment
    int xCoord;
    int yCoord;
    //Direction to reach the previous segment
    int prevDir;
    //The previous segment
    segment* prev;
    //The next segment
    segment* next;
    
   public:
    //Constructor for snake segment
    segment(int segmentX, int segmentY, segment* previous) {
      xCoord = segmentX;
      yCoord = segmentY;
      prev = previous;
      //Now set the proper direction
      if(prev == NULL) prevDir = 0;
      else if(prev->getX() > xCoord) prevDir =  1; //Segment is to the right
      else if(prev->getX() < xCoord) prevDir = -1; //Segment is to the left
      else if(prev->getY() > yCoord) prevDir =  2; //Segment is below
      else                          prevDir = -2; //Segment is above
      next = NULL;
      //Attempt to set the previous item's parent to self
      if(previous != NULL) previous->setParent(this);
    }
    //Constructor for snake segment which only depends on the previous segment and a direction
    segment(segment* previous, int dir) {
      prev = previous;
      //Extrapolate the proper X and Y coords
      if(dir == -1) {
        yCoord = prev->getY() + 1;
        xCoord = prev->getX();
        prevDir = -2;
      }
      if(dir == -2) {
        yCoord = prev->getY() - 1;
        xCoord = prev->getX();
        prevDir = -1;
      }
      if(dir == 1) {
        yCoord = prev->getY();
        xCoord = prev->getX() + 1;
        prevDir = 2;
      }
      if(dir == 2) {
        yCoord = prev->getY();
        xCoord = prev->getX() - 1;
        prevDir = 1;
      }
      next = NULL;
      //Attempt to set the previous item's parent to self
      if(previous != NULL) previous->setParent(this);
    }
    //Getters for X and Y, no setters since there's no value to them
    int getX() {
      return xCoord;
    }
    int getY() {
      return yCoord;
    }
    int getPrevDir() {
      return prevDir;
    }
    //Setters and getters for parents and children
    segment* getPrev() {
      return prev;
    }
    segment* getParent() {
      return next;
    }
    void setPrev(segment* previous) {
      //When setting previous, need to adjust accordingly
      prev = previous;
      if(prev == NULL) prevDir = 0;
      else if(prev->getX() > xCoord) prevDir =  1; //Segment is to the right
      else if(prev->getX() < xCoord) prevDir =  2; //Segment is to the left
      else if(prev->getY() > yCoord) prevDir = -1; //Segment is below
      else                          prevDir = -2; //Segment is above
    }
    void setParent(segment* parent) {
      next = parent;
    }
};

//Variable for outer use
boolean playingSnake;
//Special pause var
boolean needsRepainted;
//Var to know if we should congratulate the player
boolean newHighScoreSnake;

//Keep track of the apples eaten
int scoreSnake;

//The game timer
int gameTimer;

//The direction currently being faced
int currentDir;

//The head and tail of the snake
segment* head; segment* tail;

//The apple's location
int appleX, appleY;

//Declare the functions
void setupSnake();
void loopSnake();
void getInputSnake();
void startGameSnake();
void drawGridSnake();
void updateSnake();
void gameOverSnake();
void placeApple();

void setupSnake()
{

  playingSnake = true;
  newHighScoreSnake = false;

  //Create a head segment and three kids for it
  //Start at the tail
  tail = new segment(8, 8, NULL);
  segment* mid = new segment(9, 8, tail);
  head = new segment(9, 9, mid);
  //Put an apple somewhere on the board
  placeApple();
  scoreSnake = 0;
  gameTimer = 0;
  currentDir = 1;

  screen.fillRect(0, 0, 160, 160, TFT_BLACK);
  drawGridSnake();
}

void loopSnake()
{

  //Get and update any inputs as appropriate
  getInputSnake();

  //Increase the game timer by 1
  gameTimer++;
  //Check if we have now hit a new 'turn'
  if(gameTimer > 50) {
    //Every 5 'ticks' we want to say it's a new turn
    //Reset game timer to 0
    gameTimer = gameTimer % 5;
    //Update the gamestate
    updateSnake();
    //Draw the updated board if needed
    if(needsRepainted) drawGridSnake();
  }
  delay(2);
}

//Get input from the user
void getInputSnake() {
  //Request a direction with the joystick
  int requestedDir = 0;
  //Check for input from the grid
  if(joystick.isUp()) requestedDir = -1;
  else if(joystick.isDown()) requestedDir = -2;
  else if(joystick.isRight()) requestedDir = 1;
  else if(joystick.isLeft()) requestedDir = 2;

  //Make sure a direction was requested
  if(requestedDir != 0) {
    //Make sure it's not bumping into itself
    if(head->getPrevDir() != requestedDir) currentDir = requestedDir;
  }
}

//Put an apple in a random non-snake spot on the board
void placeApple() {
  //If we have a score of 195, give up buddy we lose
  if(scoreSnake > 194) return;
  //Otherwise, we can generate a new random location
  while(true) {
    int randX = random(14) + 1;
    int randY = random(14) + 1;
    boolean taken = false;
    //Loop through the current snake to make sure we're not hitting him
    segment* temp = head;
    while(temp != NULL) {
      if(randX == temp->getX() && randY == temp->getY()) {
        taken = true;
        Serial.println("Can't put an apple there!");
        break;
      }
      temp = temp->getPrev();
    }
    if(!taken) {
      appleX = randX;
      appleY = randY;
      break;
    }
  }
  screen.fillRect(appleX * 8 + 1, 32 + appleY * 8 + 1, 7, 7, TFT_RED);
}

//Draw the grid in its current state
void drawGridSnake() {
  //UI Output current score at the top
  screen.setTextColor(TFT_WHITE, TFT_BLACK);
  screen.drawCentreString("Current Score:", 64, 0, 2);
  screen.drawCentreString(String(scoreSnake), 64, 16, 2);
  //Completely wipe the screen
  screen.fillRect(0, 32, 128, 128, TFT_GREY);
  //Put the middle in
  screen.fillRect(8, 40, 112, 112, TFT_WHITE);
  //Put in the lines
  for(int i = 0; i < 15; i++) {
    screen.drawLine(0, 40 + 8 * i, 128, 40 + 8 * i, TFT_BLACK);
    screen.drawLine(8 + 8 * i, 32, 8 + 8 * i, 160, TFT_BLACK);
  }
  //Draw the apple
  screen.fillRect(appleX * 8 + 1, 32 + appleY * 8 + 1, 7, 7, TFT_RED);
  //Start at the head, and then draw all the bits behind
  segment* temp = head;
  while(temp->getPrev() != NULL) {
    screen.fillRect(temp->getX() * 8 + 1, 32 + temp->getY() * 8 + 1, 7, 7, TFT_GREEN);
    temp = temp->getPrev();
  }
  //Now draw the tail
  screen.fillRect(temp->getX() * 8 + 1, 32 + temp->getY() * 8 + 1, 7, 7, TFT_GREEN);
  //Mark the screen as filled
  needsRepainted = false;
}

//Drawing of the snake is done in here because it leads to less flashing
void updateSnake() {
  //UI Output current score at the top
  screen.setTextColor(TFT_WHITE, TFT_BLACK);
  screen.drawCentreString(String(scoreSnake), 64, 16, 2);
  //Move the snake one forward
  //Get a new head node
  segment* temp = new segment(head, currentDir);
  head->setParent(temp);
  head = temp;

  //Check for any collisions
  //If the X or Y coordinate are 0 or 15, then we've had a collision
  //Computation is faster if I'm a lil easier on him
  int xPos = head->getX(); int yPos = head->getY();
  if(xPos == 0 || yPos == 0 || xPos == 15 || yPos == 15) {
    Serial.println("Hit a wall");
    gameOverSnake();
    return;
  }
  //Otherwise we have to check to see if we've hit one of ourselves
  temp = head->getPrev();
  while(temp != NULL) {
    if(xPos == temp->getX() && yPos == temp->getY()) {
      Serial.println("Hit self");
      gameOverSnake();
      return;
    }
    temp = temp->getPrev();
  }
  //Otherwise we have to check to see if we're eating an apple
  if(xPos == appleX && yPos == appleY) {
    //Score goes up
    scoreSnake++;
    if(scoreSnake > highScoreSnake) {
      newHighScoreSnake = true;
    }
    //Place a new apple
    appleX = -1; appleY = -1;
    placeApple();
  }
  //If we're not, remove the tail node
  else {
    //Before deleting the tail, erase it on the board
    screen.fillRect(tail->getX() * 8 + 1, 32 + tail->getY() * 8 + 1, 7, 7, TFT_WHITE);
    tail = tail->getParent();
    delete (tail->getPrev());
    tail->setPrev(NULL);
  }
  
  //Start at the head, and then draw all the bits behind
  temp = head;
  while(temp->getPrev() != NULL) {
    screen.fillRect(temp->getX() * 8 + 1, 32 + temp->getY() * 8 + 1, 7, 7, TFT_GREEN);
    temp = temp->getPrev();
  }
  //Now draw the tail
  screen.fillRect(temp->getX() * 8 + 1, 32 + temp->getY() * 8 + 1, 7, 7, TFT_GREEN);
}

void gameOverSnake() {
  //Update the high score if necessary
  if(newHighScoreSnake) {
    highScoreSnake = scoreSnake;
  }
  //I stole this code from Tic-Tac-Toe, which I stole from minesweeper
  //I'm very resourceful.
  delay(DELAY); delay(DELAY);
  
  screen.fillRect(0, 40, 128, 80, TFT_BLACK);
  screen.setTextColor(TFT_WHITE, TFT_BLACK);
  screen.drawCentreString("Game Over!", 64, 42, 2);
  if(newHighScoreSnake) {
    screen.drawCentreString("New high score!", 64, 64, 1);
     screen.drawCentreString("Score: " + String(scoreSnake), 64, 74, 1);
  }
  else screen.drawCentreString("Score: " + String(scoreSnake), 64, 68, 2);
  screen.drawCentreString("Try Again", 64, 85, 2);
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
      //Restart
      if(yPos == 0)  {
        setupSnake();
      }
      else {
        playingSnake = false;
      }
      while(digitalRead(bluBut));
      break;
    }
  }
}
