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

</details>

