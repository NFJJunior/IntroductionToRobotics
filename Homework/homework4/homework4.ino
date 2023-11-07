//  All the segments pins
const int pinA = 13;
const int pinB = 12;
const int pinC = 11;
const int pinD = 10;
const int pinE = 9;
const int pinF = 8;
const int pinG = 7;
const int pinDP = 6;

const int segSize = 8;

const int segments[segSize] = {
    pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

byte segmentState[segSize] = {
    LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW
};

//  The directions the segment could move to
const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;

const int moveSize = 4;

//  This matrix stores the possible changes of the segments
const int moveMatrix[segSize][moveSize] = {
    //  UP, DOWN, LEFT, RIGHT
    {0, 6, 5, 1},  //  a -> 0
    {0, 6, 5, 1},  //  b -> 1
    {6, 3, 4, 7},  //  c -> 2
    {6, 3, 4, 2},  //  d -> 3
    {6, 3, 4, 2},  //  e -> 4
    {0, 6, 5, 1},  //  f -> 5
    {0, 3, 6, 6},  //  g -> 6
    {7, 7, 2, 7}   //  dp -> 7
};

int currentSegment = 7;
byte currentSegmentState = HIGH;

unsigned long lastBlinkTime = 0;
const unsigned long blinkDelay = 250;

//  Joystick move variables
const int pinX = A0;
const int pinY = A1;

int xValue = 0;
int yValue = 0;

const int minThreshold = 400;
const int maxThreshold = 640;

//  Flag that tells if the joystick has moved
bool joyMoved = false;

//  Variables added for debounce. Before I used this, sometimes there were multiple moves instead of one.
unsigned long lastMoveTime = 0;
const unsigned long moveDelay = 50;

//  Joystic button variables
const int pinSW = 2;

volatile bool buttonPressed = false;

volatile unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 200;

//  Flag that tells if the segments have reseted
volatile bool reset = false;

const unsigned long resetDelay = 2000;

void setup() {
    for (int i = 0; i < 8; i++) {
        pinMode(segments[i], OUTPUT);
    }

    pinMode(pinX, INPUT);
    pinMode(pinY, INPUT);
    pinMode(pinSW, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(pinSW), pressButtonInterrupt, FALLING);

    Serial.begin(9600);
}

void pressButtonInterrupt() {
    static unsigned long interruptTime = 0;

    interruptTime = micros();

    //  After I hold the button in order to reset, there is a chance that the button will catch a FALLING interrupt.
    //  In order to prevent that, the button can't be pressed if a reset has happened.
    if (!reset && (interruptTime - lastInterruptTime) >= (debounceDelay * 1000)) {
        buttonPressed = true;
    }

    reset = false;

    lastInterruptTime = interruptTime;
}

void pressButton() {
    if (buttonPressed) {
        segmentState[currentSegment] = !segmentState[currentSegment];

        buttonPressed = false;
    }

    if (!reset && (micros() - lastInterruptTime) >= (resetDelay * 1000) && (digitalRead(pinSW) == LOW)) {
        for (int i = 0; i < 8; i++) {
            segmentState[i] = LOW;
        }
        currentSegment = 7;

        reset = true;
    }
}

void moveSegment(const int move) {
    //  When I move the segment, I put the current state on high and reset the blink timer
    if (currentSegment != moveMatrix[currentSegment][move]) {
        currentSegmentState = HIGH;
        lastBlinkTime = millis();
    }

    currentSegment = moveMatrix[currentSegment][move];
}

void readJoyValues() {
    xValue = analogRead(pinX);
    yValue = analogRead(pinY);

    if (xValue < minThreshold && joyMoved == false && (millis() - lastMoveTime) >= moveDelay) {
        moveSegment(LEFT);

        joyMoved = true;
    }

    if (xValue > maxThreshold && joyMoved == false && (millis() - lastMoveTime) >= moveDelay) {
        moveSegment(RIGHT);

        joyMoved = true;
    }

    if (yValue < minThreshold && joyMoved == false && (millis() - lastMoveTime) >= moveDelay) {
        moveSegment(UP);

        joyMoved = true;
    }

    if (yValue > maxThreshold && joyMoved == false && (millis() - lastMoveTime) >= moveDelay) {
        moveSegment(DOWN);

        joyMoved = true;
    }

    if (xValue >= minThreshold && xValue <= maxThreshold && yValue >= minThreshold && yValue <= maxThreshold) {
        joyMoved = false;

        lastMoveTime = millis();
    }
}

void writeSegments() {
    for (int i = 0; i < segSize; i++) {
        digitalWrite(segments[i], segmentState[i]);
    }

    if ((millis() - blinkDelay) >= lastBlinkTime) {
        currentSegmentState = !currentSegmentState;
        lastBlinkTime = millis();
    }

    digitalWrite(segments[currentSegment], currentSegmentState);
}

void loop() {
    pressButton();
    readJoyValues();
    writeSegments();
}
