#include <MULTI_HX711.h>

// Pin Arrays für die HX711-Instanz
byte out_pins[] = {D5, D6, D1, D2}; // Daten-Pins
byte clock_pins[] = {D7, D3};      // Clock-Pins
const byte num_out = sizeof(out_pins) / sizeof(byte);     // Anzahl der Daten-Pins
const byte num_clk = sizeof(clock_pins) / sizeof(byte); // Anzahl der Clock-Pins
MULTI_HX711 hx711(out_pins, clock_pins, num_out, num_clk);
/*
MULTI_HX711 hx711(D5, D7);
byte num_out = 1;
*/

void setup() {
  Serial.begin(9600);
  while(!Serial); 
  hx711.setTare(10,10);
}

void loop() {
  if (hx711.readyToSend()) {
    // Read the data and store it in an array
    uint32_t* data = hx711.readTare();

    // Print the data
    for (int i = 0; i < num_out; i++) {
      Serial.print(" Data[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.print(data[i]);
    }
   Serial.println("");
   delay(1000);
  }
}
