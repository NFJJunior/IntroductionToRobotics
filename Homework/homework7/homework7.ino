#include "LedControl.h"

//  Pins for driver
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;

const byte matrixSize = 16;
byte matrixBrightness = 4;

//  Initialize driver
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

//  Matrix with 62.5% walls
byte matrix[matrixSize][matrixSize] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

struct position {
    int x;
    int y;
};

//  Player variables
position playerPosition;
byte playerLedState = HIGH;

//  Joystick X and Y pins
const byte joyXPin = A0;
const byte joyYPin = A1;

//  The four directions the player can go to
const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;

//  Joystick button variables
const byte joyButtonPin = 2;

//  Bomb variables
position bombPosition = {-1, -1};
byte bombLedState = HIGH;

//  The time the bomb was placed. There can only be one bomb at the time
unsigned long bombPlacedTime;

//  Flag that tells us if the game has been lost
bool gameLost = false;

//  Fog of view
const byte fogOfViewSize = 8;

//  The position of top-left corner of the fog of view
position corner = {0, 0};

//  Initialize display
void setupDisplay() {
    lc.shutdown(0, false);
    lc.setIntensity(0, matrixBrightness);
    lc.clearDisplay(0);
}

//  Randomize the matrix using the Fisher-Yates shuffle
void randomizeMap() {

    //  Because the pin 0 is not connected we consider its value as a random seed to the generator
    randomSeed(analogRead(0));

    //  Fisher-Yates shuffle
    //  https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle

    for (int i = matrixSize * matrixSize - 1; i >= 0; i--) {
        int j = random(i);

        //  I switch the elements using:
        //  a = a ^ b;
        //  b = a ^ b;
        //  a = a ^ b;
        matrix[i / matrixSize][i % matrixSize] ^= matrix[j / matrixSize][j % matrixSize];
        matrix[j / matrixSize][j % matrixSize] ^= matrix[i / matrixSize][i % matrixSize];
        matrix[i / matrixSize][i % matrixSize] ^= matrix[j / matrixSize][j % matrixSize];
    }
}

//  Function that finds the initial player position.
//  It searches for an empty space that has at least one adjacent horizontal empty space and one adjacent vertical empty space
//  This guarantees that the player can explode a bomb safely
void findPlayerPostion() {
    bool x_axis = false, y_axis = false;

    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            if (matrix[row][col] == 0) {
                if (matrix[(row - 1 + matrixSize) % matrixSize][col] == 0 || matrix[(row + 1) % matrixSize][col] == 0) {
                    x_axis = true;
                }
                if (matrix[row][(col - 1 + matrixSize) % matrixSize] == 0 || matrix[row][(col + 1) % matrixSize] == 0) {
                    y_axis = true;
                }

                if (x_axis && y_axis) {
                    playerPosition.x = row;
                    playerPosition.y = col;

                    return;
                }

                x_axis = false;
                y_axis = false;
            }
        }
    }
}

void setup() {
    pinMode(joyXPin, INPUT);
    pinMode(joyYPin, INPUT);
    pinMode(joyButtonPin, INPUT_PULLUP);

    setupDisplay();
    randomizeMap();
    findPlayerPostion();

    Serial.begin(9600);
}

//  The four moves the player can make
void movePlayer(const int move) {
    static int temp;

    switch(move) {
        case UP: {
            //  If the player is at line 0 and he goes up
            //  He will arrive at line (0 - 1 + 16) % 16 = 15;
            //  If the player is at line 8 and he goes up
            //  He will arrive at line (8 - 1 + 16) % 16 = 7;
            temp = (playerPosition.x - 1 + matrixSize) % matrixSize;

            if (matrix[temp][playerPosition.y] == 0) {
                playerPosition.x = temp;
            }

            break;
        }
        case DOWN: {
            temp = (playerPosition.x + 1) % matrixSize;

            if (matrix[temp][playerPosition.y] == 0) {
                playerPosition.x = temp;
            }

            break;
        }
        case LEFT: {
            temp = (playerPosition.y - 1 + matrixSize) % matrixSize;

            if (matrix[playerPosition.x][temp] == 0) {
                playerPosition.y = temp;
            }

            break;
        }
        case RIGHT: {
            temp = (playerPosition.y + 1) % matrixSize;

            if (matrix[playerPosition.x][temp] == 0) {
                playerPosition.y = temp;
            }

            break;
        }
    }
}

//  Read joystick X and Y values with debounce
void readJoyValues() {
    static int xValue;
    static int yValue;

    static const int minThreshold = 400;
    static const int maxThreshold = 640;

    //  Flag that tells us if the joy has moved
    static bool joyMoved = false;

    //  Variables for debounce
    static const unsigned int moveDelay = 50;
    static unsigned long lastMoveTime;

    xValue = analogRead(joyXPin);
    yValue = analogRead(joyYPin);

    if (xValue < minThreshold && joyMoved == false && (millis() - lastMoveTime) >= moveDelay) {
        movePlayer(LEFT);

        joyMoved = true;
    }

    if (xValue > maxThreshold && joyMoved == false && (millis() - lastMoveTime) >= moveDelay) {
        movePlayer(RIGHT);

        joyMoved = true;
    }

    if (yValue < minThreshold && joyMoved == false && (millis() - lastMoveTime) >= moveDelay) {
        movePlayer(UP);

        joyMoved = true;
    }

    if (yValue > maxThreshold && joyMoved == false && (millis() - lastMoveTime) >= moveDelay) {
        movePlayer(DOWN);

        joyMoved = true;
    }

    if (xValue >= minThreshold && xValue <= maxThreshold && yValue >= minThreshold && yValue <= maxThreshold) {
        joyMoved = false;

        lastMoveTime = millis();
    }
}

//  Read joystick button values with debounce
void readButton() {
    static byte buttonState;

    static byte reading;
    static byte lastReading;

    static const unsigned int debounceDelay = 50;
    static unsigned long lastDebounceTime;

    reading = digitalRead(joyButtonPin);

    if (reading != lastReading) {
        lastDebounceTime = millis();
    }

    if (millis() - lastDebounceTime > debounceDelay) {

        //  If the state has indeed changed and the readng is low and no bomb has been placed yet put down a bomb
        if (reading != buttonState && reading == LOW && bombPosition.x == -1) {
            bombPosition = playerPosition;

            matrix[bombPosition.x][bombPosition.y] = 2;

            bombPlacedTime = millis();
        }

        buttonState = reading;
    }

    lastReading = reading;
}

void changePlayerLed() {
    static const unsigned int blinkDelay = 250;
    static unsigned long lastBlinkTime;

    if (millis() - lastBlinkTime >= blinkDelay) {
        playerLedState = !playerLedState;
        lastBlinkTime = millis();
    }
}

//  Destroy position p.
//  Remove the wall and if the player is here, end the game
void destroy(position p) {
    matrix[p.x][p.y] = 0;

    if (p.x == playerPosition.x && p.y == playerPosition.y) {
        gameLost = true;
    }
}

//  Change bomb led
//  If the bomb explodes, destroy all adjacent cells
void bombLogic() {
    static const unsigned int bombDelay = 2000;

    static const unsigned int blinkDelay = 75;
    static unsigned long lastBlinkTime;

    static position temp;

    if (millis() - lastBlinkTime >= blinkDelay) {
        bombLedState = !bombLedState;
        lastBlinkTime = millis();
    }

    if (bombPosition.x != -1) {
        if (millis() - bombDelay >= bombPlacedTime) {
            temp = bombPosition;
            destroy(temp);

            temp.x = (bombPosition.x - 1 + matrixSize) % matrixSize;
            destroy(temp);

            temp.x = (bombPosition.x + 1) % matrixSize;
            destroy(temp);

            temp = bombPosition;
            temp.y = (bombPosition.y - 1 + matrixSize) % matrixSize;
            destroy(temp);

            temp.y = (bombPosition.y + 1) % matrixSize;
            destroy(temp);

            bombPosition = {-1, -1};
        }
    }
}

//  Find the top-left corner position for the fog of view
void findCorner() {
    corner.x = min(max(0, playerPosition.x - 3), matrixSize - fogOfViewSize);
    corner.y = min(max(0, playerPosition.y - 3), matrixSize - fogOfViewSize);
}

//  Write matrix
void writeDisplay() {
    //  If the game is lost, show nothing
    if (gameLost) {
        for (int row = 0; row < fogOfViewSize; row++) {
            for (int col = 0; col < fogOfViewSize; col++) {
                 lc.setLed(0, row, col, LOW);
            }
        }
    }
    else {
        findCorner();

        for (int row = 0; row < fogOfViewSize; row++) {
            for (int col = 0; col < fogOfViewSize; col++) {
                //  If the player is on this positon, write the playerLedState
                if (corner.x + row == playerPosition.x && corner.y + col == playerPosition.y) {
                    lc.setLed(0, row, col, playerLedState);
                }
                //  If the bomb is on this positon, write the bombLedState
                else if (corner.x + row == bombPosition.x && corner.y + col == bombPosition.y) {
                    lc.setLed(0, row, col, bombLedState);
                }
                else {
                    lc.setLed(0, row, col, matrix[corner.x + row][corner.y + col]);
                }
            }
        }
    }
}

void loop() {
    readJoyValues();
    readButton();
    changePlayerLed();
    bombLogic();
    writeDisplay();
}