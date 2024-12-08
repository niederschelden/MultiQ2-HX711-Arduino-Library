#include <MULTI_HX711.h>
#include <Arduino.h>

//Global dummy zur initialisierung im Setup
MULTI_HX711 hx711;

void setup_single() {
  
  hx711.init(D5, D7);
  delay(500);//Aufwärmen vor dem Tara
  hx711.setTare(10, 10); //Tara
  //Umrechnung in Kilogramm
  hx711.setFactor(13279);
  Serial.println(F("Setup Complete."));
}


void calculateJoystick(int32_t* data, int32_t total, uint8_t& x, uint8_t& y) {
    // Ensure total is valid to avoid division by zero
    if (total == 0) {
        x = 128; // Center
        y = 128; // Center
        return;
    }

    // Berechne X- und Y-Werte
    int32_t x_raw = data[1] + data[2]; // Rechts
    int32_t y_raw = data[1] + data[0]; // Vorne

    // Mappe die Werte auf den Joystick-Bereich (0–255)
    x = map(x_raw, 0, total, 0, 255);
    y = map(y_raw, 0, total, 0, 255);
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

void setup() {
  Serial.begin(115200);
  while (!Serial);
  //setup_single();
  setup_multi();
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
    int32_t* data = hx711.readTareFactorDecimal(2);
    int32_t total = 0;

    // Summiere die Daten
    for (int i = 0; i < hx711.getNumOut(); i++) {
        total += data[i];
    }

    // Berechne Joystick-Werte
    uint8_t x, y;
    calculateJoystick(data, total, x, y);

    // Ausgabe der Joystick-Werte
    Serial.print("Joystick X: ");
    Serial.println(x);
    Serial.print("Joystick Y: ");
    Serial.println(y);

    delay(1000);
  }
  /*
  if (hx711.readyToSend()) {
    // Read the data and store it in an array
    int32_t* data = hx711.readTareFactorDecimal(2); // Skaliert für 2 Dezimalstellen
    // summiere
    float gesamt = 0.0; // Verwende float für die Summierung
    // Print the data
    for (int i = 0; i < hx711.getNumOut(); i++) {
      float scaledValue = data[i] / 100.0; // Werte mit 2 Dezimalstellen skalieren
      Serial.print(F(" Data["));
      Serial.print(i);
      Serial.print(F("]: "));
      Serial.print(scaledValue, 2); // Ausgabe mit 2 Dezimalstellen
      gesamt += scaledValue;
    }
    Serial.print(F(" In Summe: "));
    Serial.println(gesamt, 2); // Gesamtsumme mit 2 Dezimalstellen
    delay(1000);
  }
  */
}
