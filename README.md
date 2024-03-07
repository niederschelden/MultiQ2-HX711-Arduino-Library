# Work in progress
I am attempting to enhance this library to enable the usage of multiple HX711 modules with only one clock pin for all modules. All credits for the original work go to Scott Russel; any errors are solely my responsibility.
This library is a fork of the [Queuetue HX711 Library](https://github.com/queuetue/Q2-HX711-Arduino-Library).

RIGHT NOW THIS WILL COMPILE, BUT IS NOT EVEN TESTED! STILL WORKING ON THE FIRST ATTEMPT

| **To-Do**                                    | **Idea Credits**            | **Status** |
|----------------------------------------------|-----------------------------|------------|
| Understand Arduino Librarys | [Wolles Elektronikkiste](https://wolles-elektronikkiste.de/en/creating-libraries-and-classes-part-i)| May GitHub Copilot bless me |
| Modify class definition of the Source Library to use arrays        | [Queuetue HX711 Library](https://github.com/queuetue/Q2-HX711-Arduino-Library)   | Done   |
| Process sensor data bit by bit           | [Wolles Elektronikkiste](https://wolles-elektronikkiste.de/en/strain-gauges) | Done     |
| Add an atomic block for critical operations  | [bogde](https://github.com/bogde/HX711/blob/master/src/HX711.cpp)               | Implementet but untested      |
| Implement a tare method                      |                         | Done       |
| Keep the code simple                         |               | NeverDone      |


## Multi HX711 Arduino Library

Fork of Queuetue HX711 Library Copyright (c) 2015 Scott Russell (scott@queuetue.com), released under the MIT license.  
See the LICENSE file for licensing details.

### A simple Arduino driver for multiple HX711 ADC.

The HX711 is a low-cost strain gauge amplifier produced by Avia Semiconductor.  Breakout boards are available for it by many producers, including the [Sparkfun 13230](https://www.sparkfun.com/products/13230).

The HX711 communicates with a non-i2c compliant two wire protocol and provides an all-in-one solution to load cell amplification with relatively low noise.

This library provides the code required to use an Arduino, the HX711 module and a strain gauge load cell to build a scale, force gauge or many other pressure or force sensitive projects.

### Class
The MULTI_HX711 class accepts two parameters during initialization: the pin for data transmission (Output) and the pin for readiness signaling (Clock). 
Additionally, it can also accept two arrays of pin numbers for more advanced setups. Finally there is a dummy constructor class that expects you to call init() by hand.

### Functions

| Function       | Description                                                            | Return Type |
|----------------|------------------------------------------------------------------------|-------------|
| MULTI_HX711    | Constructor for the MULTI_HX711 class.                                 | N/A         |
| ~MULTI_HX711   | Destructor for the MULTI_HX711 class.                                  | N/A         |
| init           | Initializes the HX711 with a single output and clock pin.              | void        |
| init           | Initializes the HX711 with arrays of output and clock pins.            | void        |
| readyToSend    | Checks if the HX711 is ready to send data.                             | bool        |
| setGain        | Sets the gain of the HX711 amplifier.                                  | void        |
| read           | Reads the raw data from the HX711.                                     | uint32_t*   |
| readTare       | Reads the raw data from the HX711 after applying the tare.             | uint32_t*   |
| readTareKilo   | Reads the data from the HX711 after applying the tare and converting it to kilograms. | float*    |
| setTare        | Sets the tare values for subsequent readings.                          | void        |
| setFactor      | Sets the conversion factor for each output pin.                        | void        |
| setFactor      | Sets the conversion factors for all output pins using an array.        | void        |
| getNumOut      | Returns the number of output pins configured in the HX711 instance.    | byte        |



### Example 
Here is a simple example of using the HX711 

```cpp
#include <MULTI_HX711.h>

//Global dummy zur initialisierung im Setup
MULTI_HX711 hx711;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  //setup_single();
  setup_multi();
}

void setup_single() {
  
  hx711.init(D5, D7);
  delay(500);//Aufwärmen vor dem Tara
  hx711.setTare(10, 10); //Tara
  //Umrechnung in Kilogramm
  hx711.setFactor(13279);
  Serial.println(F("Setup Complete."));
}


void setup_multi() {
  // Pin Arrays für die HX711-Instanz
  byte out_pins[] = {D5, D6, D1, D2}; // Daten-Pins
  byte clock_pins[] = {D7, D3};      // Clock-Pins
  // Ende Pin Arrays
  hx711.init(out_pins, clock_pins, sizeof(out_pins) / sizeof(byte), sizeof(clock_pins) / sizeof(byte));
  delay(500);//Aufwärmen vor dem Tara
  hx711.setTare(10, 10); //Tara
  //Umrechnung in Kilogramm
  uint16_t factor[] = {13279, 14250, 13079, 14278}; //ausprobierte Werte
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
