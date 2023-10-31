#include <ArduinoQueue.h>

const int NUMBER_OF_FLOORS = 3;

const int BUTTON_PIN[NUMBER_OF_FLOORS] = {2, 3, 4};
const int LED_PIN[NUMBER_OF_FLOORS] = {11, 12, 13};

//  Variables for button debouncing
byte buttonState[NUMBER_OF_FLOORS] = {HIGH, HIGH, HIGH};

byte reading[NUMBER_OF_FLOORS] = {HIGH, HIGH, HIGH};
byte lastReading[NUMBER_OF_FLOORS] = {HIGH, HIGH, HIGH};

unsigned long lastDebounceTime[NUMBER_OF_FLOORS] = {0, 0 ,0};
unsigned int debounceDelay = 50;

//  Elevator logic 
unsigned int currentFloor = 0;

bool inQueue[NUMBER_OF_FLOORS] = {false, false, false};
ArduinoQueue<int> queue(2 * NUMBER_OF_FLOORS);  //  I didn't find an elegant solution for this. The middle floor could be in queue two times

enum state {IDLE, WAIT, MOVE};
state elevatorState = IDLE;

const unsigned int waitTime = 1000;  //  The time the elevator waits until the door closes
const unsigned int floorTime = 100;  //  The time the lift will be at a floor in transit to another one. If the lift is going from floor 0 to floor 2, the led at floor 1 will be lit for 100 ms.
const unsigned int moveTime = 1000;  //  How much it takes the lift to go from a floor to another

unsigned long nextActionTime = 0;  //  The time at which the next action should happen

bool waitFlag = true;  //  Flag that tell us if we should stop at a floor

//  Elevator LED
const int ELEVATOR_LED_PIN = {10};
byte elevatorLedState = HIGH;

unsigned long lastBlinkTime = 0;
unsigned int blinkDelay = 100;

void setup() {
    for (int i = 0; i < 3; i++) {
        pinMode(BUTTON_PIN[i], INPUT_PULLUP);  //  Pullup internal resistor
        pinMode(LED_PIN[i], OUTPUT);
    }

    pinMode(ELEVATOR_LED_PIN, OUTPUT);
}

void readButtons() {
    for (int i = 0; i < 3; i++) {
        reading[i] = digitalRead(BUTTON_PIN[i]);

        if (reading[i] != lastReading[i]) {
            lastDebounceTime[i] = millis();
        }

        if ((millis() - lastDebounceTime[i]) >= debounceDelay) {
            if (reading[i] != buttonState[i]) {
                buttonState[i] = reading[i];

                if (buttonState[i] == HIGH && (elevatorState == MOVE || currentFloor != i)) {
                    if (!inQueue[i]) {
                        queue.enqueue(i);
                    }

                    inQueue[i] = true;
                }
            }
        }
        lastReading[i] = reading[i];
    }
}

void elevatorLogic() {
    //  If there are elements in queue, but their value for inQueue vector is false, the element will be popped
    while(!queue.isEmpty() && !inQueue[queue.getHead()]) {
        queue.dequeue();
    }

    switch (elevatorState) {
        //  The elevator is stationary. If there is no item in the queue, he will remain stationary.
        case IDLE: {
            if (queue.isEmpty()) {
                return;
            }

            nextActionTime = millis() + floorTime;
            if (waitFlag) {
                nextActionTime += waitTime;
            }
            waitFlag = false;

            elevatorState = WAIT;

            break;
        }
        case WAIT: {
            //  The elevator is waiting to close the doors
            if (millis() >= nextActionTime) {
                nextActionTime = millis() + moveTime;
                elevatorState = MOVE;
            }

            break;
        }
        case MOVE: {
            //  The elevator is moving
            if (millis() >= nextActionTime) {
                if (queue.getHead() > currentFloor) {
                    currentFloor ++;
                }
                else {
                    currentFloor --;
                }

                if (inQueue[currentFloor]) {
                    waitFlag = true;
                }
                inQueue[currentFloor] = false;

                if (queue.getHead() == currentFloor) {
                    queue.dequeue();
                }

                elevatorState = IDLE;
            }

            break;
        }
    }
}

void writeLeds() {
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_PIN[i], LOW);
    }

    if (elevatorState != MOVE) {
        digitalWrite(LED_PIN[currentFloor], HIGH);
    }

    if (elevatorState == MOVE) {
        if (millis() - blinkDelay >= lastBlinkTime) {
            elevatorLedState = !elevatorLedState;
            lastBlinkTime = millis();
        }
    }
    else {
        elevatorLedState = HIGH;
    }

    digitalWrite(ELEVATOR_LED_PIN, elevatorLedState);
}

void loop() {
    readButtons();
    elevatorLogic();
    writeLeds();
}
