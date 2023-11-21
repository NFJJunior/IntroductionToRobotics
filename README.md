# IntroductionToRobotics
Resources for the IntroductionToRobotics course, taken in the 3rd year of my bachelor's degree in Computer Science at Faculty of Matematics and Computer Science, University of Bucharest.

In this repository, I will upload the homeworks and final projects. Each homework includes requirements, implementation details, code, a circuit diagram and a demo video.

## Homework 1

<details>
<summary>See details</summary>
<br>

The task for the first homework was setting up the github repository for the course.

</details>

## Homework 2

<details>
<summary>See details</summary>
<br>

### Components
* RGB LED (at least 1)
* Potentiometers (at least 3)
* Resistors and wires as needed

### Technical Task
Use a separate potentiometer for controlling each color of the RGB LED (Red, Green, and Blue). This control must leverage digital electronics. Specifically, you need to read the potentiometer’s value with Arduino and then write a mapped value to the LED pins.
*

### Setup
![Setup](https://github.com/NFJJunior/IntroductionToRobotics/blob/main/Homework/homework2/SetupHM2.jpeg)

### [Code](https://github.com/NFJJunior/IntroductionToRobotics/blob/main/Homework/homework2/homework2.ino)

### [Demo](https://www.youtube.com/shorts/Zw2YsA6ZSdo)

</details>

## Homework 3

<details>
<summary>See details</summary>
<br>

### Components
* LEDs (at least 4: 3 for the floors and 1 for the elevator’s operational state)
* Buttons (at least 3 for floor calls)
* Resistors and wires as needed

### Technical Task
Design a control system that simulates a 3-floor elevator using the Arduino platform.
* LED Indicators: Each of the 3 LEDs should represent one of the 3 floors.
The LED corresponding to the current floor should light up. Additionally,
another LED should represent the elevator’s operational state. It should
blink when the elevator is moving and remain static when stationary.
* Implement 3 buttons that represent the call buttons from the
3 floors. When pressed, the elevator should simulate movement towards
the floor after a short interval (2-3 seconds).
* State Change & Timers: If the elevator is already at the desired floor,
pressing the button for that floor should have no effect. Otherwise, after
a button press, the elevator should ”wait for the doors to close” and then
”move” to the corresponding floor. If the elevator is in movement, it
should either do nothing or it should stack its decision (get to the first
programmed floor, open the doors, wait, close them and then go to the
next desired floor).

### Implementation details
* I tried to emulate a reallife elevator. I used a queue and a vector in order to store the floors order of arrival. Also, if the next floor in the queue is 2 and the button for floor 1 was pressed as well, the elevator will stop at floor 1 first before arriving to floor 2.
* I used the ArduinoQueue Library.

### Setup
![Setup](https://github.com/NFJJunior/IntroductionToRobotics/blob/main/Homework/homework3/SetupHM3.jpeg)
### [Code](https://github.com/NFJJunior/IntroductionToRobotics/blob/main/Homework/homework3/homework3.ino)

### [Demo](https://www.youtube.com/shorts/aPiWmkTi4iA)

</details>

## Homework 4

<details>
<summary>See details</summary>
<br>

### Components
* 1 7-Segments Display
* 1 Joystick
* Resistors and wires as needed

### Technical task
Use the joystick to control the position of the segment and ”draw” on the display.

The initial position should be on the DP. The current
position always blinks (irrespective of the fact that the segment is on or
off). Use the joystick to move from one position to neighbors (see table for
corresponding movement). Short pressing the button toggles the segment
state from ON to OFF or from OFF to ON. Long pressing the button
resets the entire display by turning all the segments OFF and moving the
current position to the decimal point.

### Setup
![Setup](https://github.com/NFJJunior/IntroductionToRobotics/blob/main/Homework/homework4/SetupHM4.jpeg)
### [Code](https://github.com/NFJJunior/IntroductionToRobotics/blob/main/Homework/homework4/homework4.ino)

### [Demo](https://www.youtube.com/watch?v=OCDyvGfrzfE)

</details>

## Homework 5

<details>
<summary>See details</summary>
<br>
  
### Components
* 1 4 Digit 7-Segments Display
* 3 Buttons
* Resistors and wires as needed

### Technical task
Implemented a stopwatch that counts in 10ths of a second and has a lap functionality.

There are 3 buttons: Start/Stop, Save lap/Cycle throught laps, Reset.

### Workflow
1. The display starts in the IDLE mode showing 000.0. When pressing the Start button, the timing will start.
When pressing the Laps button, the display will show the saved laps if there are anyand will enter LAPS mode. When pressing the Reset button
nothing happens.
2. During the time counter, the Start button will stop the count. The Laps button will record the current lap. The Reset button will do nothing.
3. During PAUSE Mode, the Start button will start counting from where it left off.
The Laps button will do norhing. The Reset button will reset the timer to 000.0
4. During Laps Mode, the Start button will do nothing. The Laps button will cycle through every recorded lap. The Reset button will reset the laps
and will return the display to the IDLE mode.

### Setup
![Setup](https://github.com/NFJJunior/IntroductionToRobotics/blob/main/Homework/homework5/SetupHM5.jpeg)
### [Code](https://github.com/NFJJunior/IntroductionToRobotics/blob/main/Homework/homework5/homework5.ino)

### [Demo](https://www.youtube.com/watch?v=EuUrEtstUUg)

</details>

## Homework 6

<details>
<summary>See details</summary>
<br>
  
### Components
* Ultrasonic Sensor (HC-SR04)
* LDR (Light-Dependent Resistor)
* RGB LED
* Resistors and wires as needed

### Technical task
Developed a ”Smart Environment Monitor and Logger” using Arduino. This system utilizes various sensors to gather environmental data, log this data into
EEPROM, and provide both visual feedback via an RGB LED and user interaction through a Serial Menu. The project focuses on integrating sensor readings,
memory management, Serial Communication and the general objective of building a menu.

### Setup
![Setup](https://github.com/NFJJunior/IntroductionToRobotics/blob/main/Homework/homework6/SetupHM6.jpeg)
### [Code](https://github.com/NFJJunior/IntroductionToRobotics/blob/main/Homework/homework6/homework6.ino)

### [Demo](https://www.youtube.com/watch?v=1APCul9bQxs)

</details>

