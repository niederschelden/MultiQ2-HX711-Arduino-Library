# Work in progress
I am attempting to enhance this library to enable the usage of multiple HX711 modules with only one clock pin for all modules. All credits for the original work go to Scott Russel; any errors are solely my responsibility.
This library is a fork of the [Queuetue HX711 Library](https://github.com/queuetue/Q2-HX711-Arduino-Library).

RIGHT NOW THIS WILL NOT COMPILE! STILL WORKING ON THE FIRST ATTEMPT

| **To-Do**                                    | **Idea Credits**            | **Status** |
|----------------------------------------------|-----------------------------|------------|
| Modify class definition to use arrays        |                        | work in progress       |
| Process sensor data in parallel              | [Wolles Elektronikkiste](https://wolles-elektronikkiste.de/dehnungsmessstreifen)  | Todo       |
| Add an atomic block for critical operations  | [bogde](https://github.com/bogde/HX711/blob/master/src/HX711.cpp)               | Todo       |
| Implement a tare method                      |                         | Todo       |
| Keep the code simple                         |               | Todo       |


## Multi HX711 Arduino Library

Fork of Queuetue HX711 Library Copyright (c) 2015 Scott Russell (scott@queuetue.com), released under the MIT license.  
See the LICENSE file for licensing details.

### A simple Arduino driver for multiple HX711 ADC.

The HX711 is a low-cost strain gauge amplifier produced by Avia Semiconductor.  Breakout boards are available for it by many producers, including the [Sparkfun 13230](https://www.sparkfun.com/products/13230).

The HX711 communicates with a non-i2c compliant two wire protocol and provides an all-in-one solution to load cell amplification with relatively low noise.

This library provides the code required to use an Arduino, the HX711 module and a strain gauge load cell to build a scale, force gauge or many other pressure or force sensitive projects.

### Class
The MULTI_HX711 class accepts two parameters during initialization: the pin for data transmission (Output) and the pin for readiness signaling (Clock). 
Additionally, it can also accept two arrays of pin numbers for more advanced setups.


### Functions
| Function     | Description                                                            |
|--------------|------------------------------------------------------------------------|
| read         | Returns a long integer representing the current value of the HX711.     |
| readyToSend  | Returns a boolean value indicating whether the HX711 is ready to send data. |
| setGain  | {128 = Channel A, 64 = Channel A, 32 = Channel B}; Returns notin'|


### Example 1
Here is a simple example of using the HX711 on pins 2 and 3 to read a strain gauge and print its current value:

```cpp
#include <MULTI_HX711.h>

MULTI_HX711 hx711(2, 3); // Data pin: 2, Clock pin: 3

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (hx711.readyToSend()) {
    // Read the data and store it in an array
    uint32_t* data = hx711.read();

    // Print the data
    Serial.println(data[0]);
    

    // Delete the array to avoid memory leaks
    delete[] data;

    delay(500);
  }
}
```

### Example 2
Here is an example of initializing the HX711 with multiple data and clock pins (2 data pins and 2 clock pins):

```cpp
#include <MULTI_HX711.h>

byte out_pins[] = {2,4,8,9}; // Example data pins
byte clock_pins[] = {3,5}; // Example clock pins
byte num_out = sizeof(out_pins) / sizeof(out_pins[0]); // Number of data pins
byte num_clk = sizeof(clock_pins) / sizeof(clock_pins[0]); // Number of clock pins

MULTI_HX711 hx711(out_pins, clock_pins, num_out, num_clk);

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (hx711.readyToSend()) {
    // Read the data and store it in an array
    uint32_t* data = hx711.read();

    // Print the data
    for (int i = 0; i < num_out; i++) {
      Serial.print("Data ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(data[i]);
    }

    // Delete the array to avoid memory leaks
    delete[] data;

    delay(500);
  }
}
