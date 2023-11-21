#include <EEPROM.h>

//  Distance sensor
const byte trigPin = 13;
const byte echoPin = 12;
unsigned int distanceValue;

//  Light sensor 
const byte photocellPin = A0;
unsigned int lightValue;

//  LED RGB
const byte redPin = 11;
const byte greenPin = 10;
const byte bluePin = 9;

//  Menu logic
enum MenuState {
    MAIN_MENU,

    //  Submeniu 1
    SENSOR_SETTINGS,
    SENSORS_SAMPLING_INTERVAL,   //  Choosing a sample interval state
    ULTRASONIC_ALERT_THRESHOLD,  //  Choosing a min distance threshold state
    LDR_ALERT_THRESHOLD,         //  Choosing a min light threshold

    //  Submeniu 2
    RESET_LOGGER_DATA,

    //  Submeniu 3
    SYSTEM_STATUS,
    READING_SENSORS,  //  Read both sensors state

    //  Submeniu 4
    RGB_LED_CONTROL,
    MANUAL_COLOR_CONTROL,  //  Choosing what color to change
    NO_ALERT_COLOR,        //  Choosing no alert color state
    ALERT_COLOR            //  Choosing alert color state


};
MenuState menuState = MAIN_MENU;

//  Possible colors for the RGB LED
const int maxColors = 10;
const byte colors[maxColors][3] = {
    {255, 0, 0},      //  RED
    {0, 255, 0},      //  GREEN
    {0, 0, 255},      //  BLUE
    {255, 255, 0},    //  YELLOW
    {0, 255, 255},    //  CYAN
    {255, 255, 255},  //  WHITE
    {255, 20, 147},   //  DEEP_PINK
    {50, 205, 50},    //  LIME_GREEN
    {255, 0, 255},   //  MAGENTA
    {135, 206, 235}   //  SKY_BLUE
}; 

#define RED 0
#define GREEN 1
#define BLUE 2
#define YELLOW 3
#define CYAN 4
#define WHITE 5
#define DEEP_PINK 6
#define LIME_GREEN 7
#define MAGENTA 8
#define SKY_BLUE 9

//  EEPROM addresses for all saved values
//  Every data address come exactly after the next

//  Sampling interval
const int samplingIntervalAddress = 512;  //  I choose 512 instead of 0 as the first byte of the data because I thought the 0 byte was more used.
byte samplingInterval;
unsigned long lastSamplingTime = 0;

//  Ultrasonic Alert Threshold
const int distanceThresholdAddress = samplingIntervalAddress + sizeof(samplingInterval);
unsigned int distanceThreshold;

//  LDR Alert Threshold
const int lightThresholdAddress = distanceThresholdAddress + sizeof(distanceThreshold);
unsigned int lightThreshold;

const int maxSamples = 10;

//  Distance Data. An array with 10 entrees
const int distanceDataAddress = lightThresholdAddress + sizeof(lightThreshold);
unsigned int distanceData[maxSamples];

//  Light Data. An array with 10 entrees
const int lightDataAddress = distanceDataAddress + sizeof(distanceData);
unsigned int lightData[maxSamples];

//  Automatic mode
const int automaticAddress = lightDataAddress + sizeof(lightData);
byte automatic;

//  No alert color
const int noAlertColorAddress = automaticAddress + sizeof(automatic);
byte noAlertColor;

//  Alert color
const int alertColorAddress = noAlertColorAddress + sizeof(noAlertColor);
byte alertColor;

void setup() {
    Serial.begin(9600);

    //  Ultrasonic sensor
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    //  LDR sensor
    pinMode(photocellPin, INPUT);

    //  LED RGB
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    //  First menu print
    printMenu(menuState);
}

//  Gets current settings values from EEPROM
void getSettingsData() {
    EEPROM.get(samplingIntervalAddress, samplingInterval);
    EEPROM.get(distanceThresholdAddress, distanceThreshold);
    EEPROM.get(lightThresholdAddress, lightThreshold);
    EEPROM.get(automaticAddress, automatic);
    EEPROM.get(noAlertColorAddress, noAlertColor);
    EEPROM.get(alertColorAddress, alertColor);
}

void readSensors() {
    static long duration = 0;
    static unsigned int aux;  //  Auxiliary variable 

    //  Send ultrasound throught trigger
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);  //  I thought that this delay shouldn't be a problem because it is small
    digitalWrite(trigPin, LOW);

    //  Measure the time duration for the ultrasounds to get back
    duration = pulseIn(echoPin, HIGH);

    //  Calculate distance
    distanceValue = duration * 0.034 / 2;

    //  Read light value from sensor
    lightValue = analogRead(photocellPin);

    if (millis() - lastSamplingTime >= samplingInterval * 1000) {

        //  If the menu is in mode reading, print the distance and lights values
        if (menuState == READING_SENSORS) {
            Serial.print("Distance: ");
            Serial.print(distanceValue);
            Serial.print(", Light: ");
            Serial.println(lightValue);
        }

        //  Move all data, except the last one, one step further in order to make space for the new reading
        for (int i = maxSamples - 1; i >= 1; i--) {
            EEPROM.get(distanceDataAddress + sizeof(distanceData[0]) * (i - 1), aux);
            EEPROM.put(distanceDataAddress + sizeof(distanceData[0]) * i, aux);

            EEPROM.get(lightDataAddress + sizeof(lightData[0]) * (i - 1), aux);
            EEPROM.put(lightDataAddress + sizeof(lightData[0]) * i, aux);
        }

        EEPROM.put(distanceDataAddress, distanceValue);
        EEPROM.put(lightDataAddress, lightValue);

        lastSamplingTime = millis();
    }
}

void writeColor() {
    static int color;

    if (distanceValue < distanceThreshold || lightValue < lightThreshold) {
            color = alertColor;
        }
        else {
            color = noAlertColor;
        }

    if (automatic == 1) {
        if (distanceValue < distanceThreshold || lightValue < lightThreshold) {
            color = RED;
        }
        else {
            color = GREEN;
        }
    }

    analogWrite(redPin, colors[color][0]);
    analogWrite(greenPin, colors[color][1]);
    analogWrite(bluePin, colors[color][2]);
}

//  The menu will be printed after a change
void printMenu(MenuState menuState) {
    switch(menuState) {
        case MAIN_MENU: {
            Serial.println("Main menu:");
            Serial.println("1. Sensor Settings");
            Serial.println("2. Reset Logger Data");
            Serial.println("3. System Status");
            Serial.println("4. RGB LED Control");

            break;
        }
        case SENSOR_SETTINGS: {
            Serial.println("Sensor Settings:");
            Serial.println("1. Sensors Sampling Interval");
            Serial.println("2. Ultrasonic Alert Threshold");
            Serial.println("3. LDR Alert Threshold");
            Serial.println("4. Back");

            break;
        }
        case SENSORS_SAMPLING_INTERVAL: {
            Serial.println("Choose the sampling interval between 1 and 10 (sec):");

            break;
        }
        case ULTRASONIC_ALERT_THRESHOLD: {
            Serial.println("Choose the min ultrasonic threshold between 5 and 200 (cm):");

            break;
        }
        case LDR_ALERT_THRESHOLD: {
            Serial.println("Choose the min ldr thresold between 1 and 1023:");

            break;
        }
        case RESET_LOGGER_DATA: {
            Serial.println("Are you sure?");
            Serial.println("1. Yes");
            Serial.println("2. No");

            break;
        }
        case SYSTEM_STATUS: {
            Serial.println("System Status:");
            Serial.println("1. Current Sensor Readings");
            Serial.println("2. Current Sensor Settings");
            Serial.println("3. Display Logged Data");
            Serial.println("4. Back");

            break;
        }
        case READING_SENSORS: {
            Serial.println("Press anything to quit reading mode!");

            break;
        }
        case RGB_LED_CONTROL: {
            Serial.println("RGB LED Control");
            Serial.println("1. Manual Color Control");
            Serial.print("2. LED: Toggle Automatic(");
            if (automatic == 1) {
                Serial.print("ON");
            }
            else {
                Serial.print("OFF");
            }
            Serial.println(").");
            Serial.println("3. Back");

            break;
        }
        case MANUAL_COLOR_CONTROL: {
            Serial.println("Choose what color to modify:");
            Serial.println("1. No alert color");
            Serial.println("2. Alert color");

            break;
        }
        case NO_ALERT_COLOR: {
            Serial.println("Choose the no alert color:");
            Serial.println("1. Red");
            Serial.println("2. Green");
            Serial.println("3. Blue");
            Serial.println("4. Yellow");
            Serial.println("5. Cyan");
            Serial.println("6. White");
            Serial.println("7. Deep Pink");
            Serial.println("8. Lime Green");
            Serial.println("9. Magenta");
            Serial.println("10. Sky Blue");

            break;
        }
        case ALERT_COLOR: {
            Serial.println("Choose the alert color:");
            Serial.println("1. Red");
            Serial.println("2. Green");
            Serial.println("3. Blue");
            Serial.println("4. Yellow");
            Serial.println("5. Cyan");
            Serial.println("6. White");
            Serial.println("7. Deep Pink");
            Serial.println("8. Lime Green");
            Serial.println("9. Magenta");
            Serial.println("10. Sky Blue");

            break;
        }
    }

    Serial.println();
}

void menuLogic() {
    static int option;

    if (Serial.available() > 0) {
        //  I tried with try and catch the first time, but I observed that when I don't introduce ints, the result is 0.
        option = Serial.parseInt();

        switch (menuState) {
            case MAIN_MENU: {
                switch(option) {
                    //  Move to sensor settings
                    case 1: {
                        menuState = SENSOR_SETTINGS;

                        break;
                    }
                    //  Move to reset logger data
                    case 2: {
                        menuState = RESET_LOGGER_DATA;

                        break;
                    }
                    //  Move to system status
                    case 3: {
                        menuState = SYSTEM_STATUS;

                        break;
                    }
                    //  Move to rgb led control
                    case 4: {
                        menuState = RGB_LED_CONTROL;

                        break;
                    }
                    //  If the input is wrong, we ask to input again
                    default: {
                        Serial.println("Please input a valid option!");
                        Serial.println();
                    }
                }

                break;
            }
            case SENSOR_SETTINGS: {
                switch(option) {
                    //  Move to sensors sampling interval
                    case 1: {
                        menuState = SENSORS_SAMPLING_INTERVAL;

                        break;
                    }
                    //  Move to ultrasonic alert threshold
                    case 2: {
                        menuState = ULTRASONIC_ALERT_THRESHOLD;

                        break;
                    }
                    //  Move to ldr alert threshold
                    case 3: {
                        menuState = LDR_ALERT_THRESHOLD;

                        break;
                    }
                    //  Go back to main menu
                    case 4: {
                        menuState = MAIN_MENU;

                        break;
                    }
                    default: {
                        Serial.println("Please input a valid option!");
                        Serial.println();
                    }
                }

                break;
            }
            //  If the input is good, we update the memory and go back to sensor settings
            case SENSORS_SAMPLING_INTERVAL: {
                if (option < 1 || option > 10) {
                    Serial.println("Please input a valid option!");
                    Serial.println();
                }
                else {
                    EEPROM.put(samplingIntervalAddress, (byte)option);

                    Serial.println("Sensors sampling interval modified with succes!");
                    Serial.println();

                    menuState = SENSOR_SETTINGS;
                }

                break;
            }
            case ULTRASONIC_ALERT_THRESHOLD: {
                if (option < 5 || option > 200) {
                    EEPROM.put(distanceThresholdAddress, (unsigned int)option);

                    Serial.println("Min distance threshold value modified with success!");

                    menuState = SENSOR_SETTINGS;
                }
                else {
                    Serial.println("Please input a valid option!");
                }

                Serial.println();

                break;
            }
            case LDR_ALERT_THRESHOLD: {
                if (option >= 1 && option <= 1023) {
                    EEPROM.put(lightThresholdAddress, (unsigned int)option);

                    Serial.println("Min light threshold value modified with success!");

                    menuState = SENSOR_SETTINGS;
                }
                else {
                    Serial.println("Please input a valid option!");
                }

                Serial.println();

                break;
            }
            case RESET_LOGGER_DATA: {
                switch(option) {
                    //  Reset data and light data to 0
                    case 1: {
                        for (int i = 0; i < maxSamples; i++) {
                            EEPROM.put(distanceDataAddress + sizeof(distanceData[0]) * i, (unsigned int)0);
                            EEPROM.put(lightDataAddress + sizeof(lightData[0]) * i, (unsigned int)0);
                        }

                        Serial.println("Data reseted with success!");

                        menuState = MAIN_MENU;

                        break;
                    }
                    case 2: {
                        menuState = MAIN_MENU;

                        break;
                    }
                    default: {
                        Serial.println("Please input a valid option!");
                        Serial.println();
                    }
                }

                break;
            }
            case SYSTEM_STATUS: {
                switch(option) {
                    //  Move to reading sensors state
                    case 1: {
                        menuState = READING_SENSORS;

                        break;
                    }
                    //  Show settings
                    case 2: {
                        Serial.print("Sampling interval: ");
                        Serial.println(samplingInterval);
                        Serial.print("Distance threshold: ");
                        Serial.println(distanceThreshold);
                        Serial.print("Light threshold: ");
                        Serial.println(lightThreshold);
                        Serial.println();

                        break;
                    }
                    //  Show distance and light data
                    case 3: {
                        unsigned int auxDistance;
                        unsigned int auxLight;

                        for (int i = 0; i < maxSamples; i++) {
                            EEPROM.get(distanceDataAddress + sizeof(distanceData[i]) * i, auxDistance);
                            EEPROM.get(lightDataAddress + sizeof(lightData[i]) * i, auxLight);

                            Serial.print(i + 1);
                            Serial.print(": Distance = ");
                            Serial.print(auxDistance);
                            Serial.print(", Light = ");
                            Serial.println(auxLight);
                        }

                        Serial.println();

                        break;
                    }
                    //  Go back to main menu
                    case 4: {
                        menuState = MAIN_MENU;

                        break;
                    }
                    default: {
                        Serial.println("Please input a valid option!");
                        Serial.println();
                    }
                }

                break;
            }
            case READING_SENSORS: {
                menuState = SYSTEM_STATUS;

                break;
            }
            case RGB_LED_CONTROL: {
                switch(option) {
                    //  Move to manul color control
                    case 1: {
                        menuState = MANUAL_COLOR_CONTROL;

                        break;
                    }
                    //  Switch automatic variable
                    //  Update the memory
                    case 2: {
                        automatic = 1 - automatic;
                        EEPROM.put(automaticAddress, automatic);

                        break;
                    }
                    //  Go back to main menu
                    case 3: {
                        menuState = MAIN_MENU;

                        break;
                    }
                    default: {
                        Serial.println("Please input a valid option!");
                        Serial.println();
                    }
                }

                break;
            }
            //  Choose what color to change
            case MANUAL_COLOR_CONTROL: {
                switch (option) {
                    case 1: {
                        menuState = NO_ALERT_COLOR;

                        break;
                    }
                    case 2: {
                        menuState = ALERT_COLOR;

                        break;
                    }
                    default: {
                        Serial.println("Please input a valid option!");
                        Serial.println();
                    }
                }

                break;
            }
            //  Update memory of the no alert color
            case NO_ALERT_COLOR: {
                if (option >= 1 && option <= 10) {
                    EEPROM.put(noAlertColorAddress, (byte)(option - 1));

                    Serial.println("No alert color value modified with success!");

                    menuState = RGB_LED_CONTROL;
                }
                else {
                    Serial.println("Please input a valid option!");
                }

                Serial.println();

                break;
            }
            //  Update memory of the alert color
            case ALERT_COLOR: {
                if (option >= 1 && option <= 10) {
                    EEPROM.put(alertColorAddress, (byte)(option - 1));

                    Serial.println("Alert color value modified with success!");

                    menuState = RGB_LED_CONTROL;
                }
                else {
                    Serial.println("Please input a valid option!");
                }

                Serial.println();

                break;
            }
        }

        //  After a change, print the menu
        printMenu(menuState);
    }
}

void loop() {
    getSettingsData();
    readSensors();
    writeColor();
    menuLogic();
}