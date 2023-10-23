const int MIN_ANALOG_VALUE = 0;
const int MAX_ANALOG_VALUE = 1023;

const int MIN_DIGITAL_VALUE = 0;
const int MAX_DIGITAL_VALUE = 255;

const int RED_POT = A0;
const int GREEN_POT = A1;
const int BLUE_POT = A2;

const int RED_LED = 11;
const int GREEN_LED = 10;
const int BLUE_LED = 9;

unsigned int analogRedValue;
unsigned int analogGreenValue;
unsigned int analogBlueValue;

unsigned int digitalRedValue;
unsigned int digitalGreenValue;
unsigned int digitalBlueValue;

void setup() {
    pinMode(RED_POT, INPUT);
    pinMode(GREEN_POT, INPUT);
    pinMode(BLUE_POT, INPUT);

    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
}

void readPots() {
    analogRedValue = analogRead(RED_POT);
    analogGreenValue = analogRead(GREEN_POT);
    analogBlueValue = analogRead(BLUE_POT);
}

void writeLeds() {
    digitalRedValue = map(analogRedValue, MIN_ANALOG_VALUE, MAX_ANALOG_VALUE, MIN_DIGITAL_VALUE, MAX_DIGITAL_VALUE);
    digitalGreenValue = map(analogGreenValue, MIN_ANALOG_VALUE, MAX_ANALOG_VALUE, MIN_DIGITAL_VALUE, MAX_DIGITAL_VALUE);
    digitalBlueValue = map(analogBlueValue, MIN_ANALOG_VALUE, MAX_ANALOG_VALUE, MIN_DIGITAL_VALUE, MAX_DIGITAL_VALUE);

    analogWrite(RED_LED, digitalRedValue);
    analogWrite(GREEN_LED, digitalGreenValue);
    analogWrite(BLUE_LED, digitalBlueValue);
}

void loop() {
    readPots();

    writeLeds();
}
