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
