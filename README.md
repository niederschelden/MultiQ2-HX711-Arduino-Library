# Work in progress
I am attempting to enhance this library to enable the usage of multiple HX711 modules with only one clock pin for all modules. All credits for the original work go to Scott Russel; any errors are solely my responsibility.
This library is a fork of the [Queuetue HX711 Library](https://github.com/queuetue/Q2-HX711-Arduino-Library).

RIGHT NOW THIS WILL COMPILE, BUT IS NOT EVEN TESTED! STILL WORKING ON THE FIRST ATTEMPT

| **To-Do**                                    | **Idea Credits**            | **Status** |
|----------------------------------------------|-----------------------------|------------|
| Understand Arduino Librarys | [Wolles Elektronikkiste](https://wolles-elektronikkiste.de/en/creating-libraries-and-classes-part-i)| I start to understand but have a long way to go|
| Modify class definition of the Source Library to use arrays        | [Queuetue HX711 Library](https://github.com/queuetue/Q2-HX711-Arduino-Library)   | compiles; betatesting   |
| Process sensor data bit by bit           | [Wolles Elektronikkiste](https://wolles-elektronikkiste.de/en/strain-gauges) | betatesting       |
| Add an atomic block for critical operations  | [bogde](https://github.com/bogde/HX711/blob/master/src/HX711.cpp)               | not jet tested       |
| Implement a tare method                      |                         | Done       |
| Keep the code simple                         |               | need to vreate a dummy constuctor to reduce global variables       |


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

| Function       | Description                                                            |
|----------------|------------------------------------------------------------------------|
| init    | Mandatory setup for class variables. (May be called from Constructor)|
| readyToSend    | Returns a boolean value indicating whether the HX711 is ready to send data. |
| setGain        | Sets the gain of the HX711 amplifier.  {128 Channel A,64 Channel A,32 Channel B}                                 |
| read           | Reads the raw data from the HX711.                                      |
| readTare       | Reads the raw data from the HX711 after applying the tare.              |
| readTareKilo   | Reads the data from the HX711 after applying the tare and converting it to kilograms. |
| setTare        | Sets the tare values for subsequent readings.                           |
| setFactor      | Sets the conversion factor for each output pin.                         |
| getNumOut      | Returns the number of output pins configured in the HX711 instance.     |



### Example 1
Here is a simple example of using the HX711 on pins 2 and 3 to read a strain gauge and print its current value:

```cpp
#include <MULTI_HX711.h>

MULTI_HX711 hx711(D5, D7);


void setup() {
  Serial.begin(9600);
  while(!Serial); 
  delay(500);
  hx711.setTare(10,10);
  uint16_t factor[] = {13279,14250,13079,14278}; //ausprobierte Werte
  hx711.setFactor(factor);
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 't') {
      // Set tare values
      hx711.setTare(10, 10);
      Serial.println(F("Tare completed."));
    }
  }
  
  if (hx711.readyToSend()) {
    // Read the data and store it in an array
    float* data = hx711.readTareKilo();
    // Print the data
    for (int i = 0; i < hx711.getNumOut(); i++) {
      Serial.print(F(" Data["));
      Serial.print(i);
      Serial.print(F("]: "));
      Serial.print(data[i]);
    }
   delay(1000);
  }
}
```
### Example 2
Four HX711 with only two clockpins read the value after tare:
```cpp
#include <MULTI_HX711.h>

// Pin Arrays für die HX711-Instanz

byte out_pins[] = {D5, D6, D1, D2}; // Daten-Pins
byte clock_pins[] = {D7, D3};      // Clock-Pins
MULTI_HX711 hx711(out_pins, clock_pins, sizeof(out_pins) / sizeof(byte), sizeof(clock_pins) / sizeof(byte));


//MULTI_HX711 hx711(D5, D7);


void setup() {
  Serial.begin(9600);
  while(!Serial); 
  delay(500);
  hx711.setTare(10,10);
  uint16_t factor[] = {13279,14250,13079,14278}; //ausprobierte Werte
  hx711.setFactor(factor);
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 't') {
      // Set tare values
      hx711.setTare(10, 10);
      Serial.println(F("Tare completed."));
    }
  }
  
  if (hx711.readyToSend()) {
    // Read the data and store it in an array
    float* data = hx711.readTareKilo();
    // summiere
    float gesamt = 0.0;
    // Print the data
    for (int i = 0; i < hx711.getNumOut(); i++) {
      Serial.print(F(" Data["));
      Serial.print(i);
      Serial.print(F("]: "));
      Serial.print(data[i]);
      gesamt += data[i];
    }
   Serial.print(F(" In Summe: "));
   Serial.println(gesamt);
   delay(1000);
  }
}
```
### Example 3
Reducing global variables by initilaisation in the setup()
```cpp
#include <MULTI_HX711.h>

//Global dummy zur initialisierung im Setup
MULTI_HX711 hx711;

void setup() {
  Serial.begin(9600);
  while(!Serial); 
  // Pin Arrays für die HX711-Instanz
  byte out_pins[] = {D5, D6, D1, D2}; // Daten-Pins
  byte clock_pins[] = {D7, D3};      // Clock-Pins
  // Ende Pin Arrays
  hx711.init(out_pins, clock_pins, sizeof(out_pins) / sizeof(byte), sizeof(clock_pins) / sizeof(byte));
  delay(500);//Aufwärmen vor dem Tara
  hx711.setTare(10,10);//Tara
  //Umrechnung in Kilogramm
  uint16_t factor[] = {13279,14250,13079,14278}; //ausprobierte Werte
  hx711.setFactor(factor);
  Serial.println(F("Setup Complete."));
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 't') {
      // Set tare values
      hx711.setTare(10, 10);
      Serial.println(F("Tare completed."));
    }
  }
  
  if (hx711.readyToSend()) {
    // Read the data and store it in an array
    float* data = hx711.readTareKilo();
    // summiere
    float gesamt = 0.0;
    // Print the data
    for (int i = 0; i < hx711.getNumOut(); i++) {
      Serial.print(F(" Data["));
      Serial.print(i);
      Serial.print(F("]: "));
      Serial.print(data[i]);
      gesamt += data[i];
    }
   Serial.print(F(" In Summe: "));
   Serial.println(gesamt);
   delay(1000);
  }
}

```
