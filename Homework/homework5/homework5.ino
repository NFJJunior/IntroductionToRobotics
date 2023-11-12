#include <ArduinoQueue.h>

//  DS = [D]ata [S]torage - data
//  STCP = [ST]orage [C]lock [P]in - latch
//  SHCP = [SH]ift register [C]lock [P]in - clock
const byte dataPin = 12;
const byte latchPin = 11;
const byte clockPin = 10;

//  Number of display digits
const int displayNumber = 4;
const byte displayDigit[displayNumber] = {5, 6, 7, 8};

const int encodingNumber = 10;
byte encodings[encodingNumber] = {
    //  A B C D E F G DP
    B11111100,  //  0
    B01100000,  //  1
    B11011010,  //  2
    B11110010,  //  3
    B01100110,  //  4
    B10110110,  //  5
    B10111110,  //  6
    B11100000,  //  7
    B11111110,  //  8
    B11110110   //  9
};

//  Display on which the decimal point is shown
const int dpDisplay = 2;

//  Variables used for counting functions
unsigned long lastIncrement = 0;
unsigned long delayCount = 100;

//  What will be shown on display
unsigned int number = 0;

//  Possible states of the circuit
enum state {
    IDLE,      //  Doing nothing, showing 0  
    COUNTING,  //  Measuring time
    PAUSE,     //  The measure has been paused.
    LAPS       //  Cycling through laps
};

//  Current state of the circuit
state currentState = IDLE;

//  Pins for buttons
const byte startPin = 2;
const byte lapPin = 3;
const byte resetPin = 4;

//  Debounce delays used with interrupts
const unsigned long debounceDelay = 200;

//  Last time the start button was pushed
volatile unsigned long lastStartTime = 0;
volatile bool startPressed = false;

//  Last time the lap button was pushed.
//  This is for debounce.
volatile unsigned long lastLapTime = 0;
volatile bool lapPressed = false;

//  Maximum numbers of laps
const int lapsMaxSize = 4;

//  Queue structure to store the laps
ArduinoQueue<int> laps(lapsMaxSize);

//  The number at which the last lap button was pushed.
//  This is for calculating the lap times
unsigned int lastSavedLap = 0;

bool resetPressed = false;

void setup() {
    pinMode(dataPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);

    for (int i = 0; i < displayNumber; i++) {
        pinMode(displayDigit[i], OUTPUT);

        digitalWrite(displayDigit[i], LOW);
    }

    pinMode(startPin, INPUT_PULLUP);
    pinMode(lapPin, INPUT_PULLUP);
    pinMode(resetPin, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(startPin), startButtonInterrupt, FALLING);
    attachInterrupt(digitalPinToInterrupt(lapPin), lapButtonInterrupt, FALLING);
}

void startButtonInterrupt() {
    static unsigned long interruptTime = 0;

    interruptTime = micros();

    if ((interruptTime - lastStartTime) >= (debounceDelay * 1000)) {
        startPressed = true;
    }

    lastStartTime = interruptTime;
}

void lapButtonInterrupt() {
    static unsigned long interruptTime = 0;

    interruptTime = micros();

    if ((interruptTime - lastLapTime) >= (debounceDelay * 1000)) {
        lapPressed = true;
    }

    lastLapTime = interruptTime;
}

void activateDisplay(int display) {
    for (int i = 0; i < displayNumber; i++) {
        digitalWrite(displayDigit[i], HIGH);
    }

    digitalWrite(displayDigit[display], LOW);
}

void writeReg(byte encoding) {
    digitalWrite(latchPin, LOW);

    shiftOut(dataPin, clockPin, MSBFIRST, encoding);

    digitalWrite(latchPin, HIGH);
}

void writeNumber(int number) {
    for (int i = displayNumber - 1; i >= 0; i--) {
        activateDisplay(i);
        
        //  Write the correct digit, acording to encodings.
        //  If the current display is dpDisplay, show the decimal point as well
        writeReg(encodings[number % 10] | (byte)(i == dpDisplay));
        writeReg(B00000000);

        number /= 10;
    }
}

//  Read if the reset button has been pushed.
//  Doesn't matter if it catches multiple resets.
//  It does not affect the circuit
void readReset() {
    if (digitalRead(resetPin) == LOW) {
        resetPressed = true;
    }
}

//  Logic for the stopwatch
void stopwatchLogic() {
    switch(currentState) {
        case IDLE: {
            number = 0;
            lastIncrement = millis();
            lastSavedLap = number;

            if (startPressed) {
                currentState = COUNTING;

                //  If I start counting again, I clear the stored laps
                while (!laps.isEmpty()) {
                    laps.dequeue();
                }

                startPressed = false;
            }

            //  If there are laps recorded, I show the first one and than I move to LAPS state
            if(lapPressed) {
                if (!laps.isEmpty()) {
                    number = laps.getHead();
                    laps.dequeue();
                    laps.enqueue(number);

                    currentState = LAPS;
                }

                lapPressed = false;
            }

            resetPressed = false;

            break;
        }
        case COUNTING: {
            if (millis() - lastIncrement >= delayCount) {
                number = (number + 1) % 10000;
                lastIncrement = millis();
            }

            if (startPressed) {
                currentState = PAUSE;

                startPressed = false;
            }

            //  If the queue is full, I remove the first recorded lap
            if (lapPressed) {
                if (laps.isFull()) {
                    laps.dequeue();
                }
                laps.enqueue(number - lastSavedLap);

                lastSavedLap = number;

                lapPressed = false;
            }

            resetPressed = false;

            break;
        }
        case PAUSE: {
            if (startPressed) {
                currentState = COUNTING;

                startPressed = false;
            }

            lapPressed = false;

            //  Timer reseted
            if (resetPressed) {
                currentState = IDLE;

                resetPressed = false;
            }

            break;
        }
        case LAPS: {
            startPressed = false;

            if (lapPressed) {
                number = laps.getHead();
                laps.dequeue();
                laps.enqueue(number);

                lapPressed = false;
            }

            //  After I pressed the reset button again, I clear all laps
            //  than I move to IDLE state
            if (resetPressed) {
                currentState = IDLE;

                while (!laps.isEmpty()) {
                    laps.dequeue();
                }

                resetPressed = false;
            }
        }
    }
}

void loop() {
    readReset();
    stopwatchLogic();
    writeNumber(number);
}
