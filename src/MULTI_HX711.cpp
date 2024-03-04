#include <Arduino.h>
#include "MULTI_HX711.h"

MULTI_HX711::MULTI_HX711(byte output_pin, byte clock_pin) {
  byte clock_pins[] = {clock_pin}; // Erzeuge ein Array mit einem Element
  byte out_pins[] = {output_pin}; // Erzeuge ein Array mit einem Element
  init(out_pins, clock_pins, 1, 1); // Rufe die init-Methode mit den Arrays der Länge 1 auf
}

MULTI_HX711::MULTI_HX711(byte* output_pins, byte* clock_pins, byte num_out, byte num_clk) {
  init(output_pins, clock_pins, num_out, num_clk); // Rufe die init-Methode mit den übergebenen Arrays auf
}

MULTI_HX711::~MULTI_HX711() {
  // Freigabe des allokierten Speichers für die Arrays
  delete[] CLOCK_PINS;
  delete[] OUT_PINS;
}

bool MULTI_HX711::readyToSend() {
  if (!pinsConfigured) {
    // We need to set the pin mode once, but not in the constructor
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(OUT_PIN, INPUT);
    pinsConfigured = true;
  }
  return digitalRead(OUT_PIN) == LOW;
}

void MULTI_HX711::setGain(byte gain) {
  switch (gain) {
    case 128:
      GAIN = 1;
      break;
    case 64:
      GAIN = 3;
      break;
    case 32:
      GAIN = 2;
      break;
  }

  digitalWrite(CLOCK_PIN, LOW);
  read();
}

long MULTI_HX711::read() {
   while (!readyToSend());

  byte data[3];

  for (byte j = 3; j--;) {
      data[j] = shiftIn(OUT_PIN,CLOCK_PIN, MSBFIRST);
  }

  // set gain
  for (int i = 0; i < GAIN; i++) {
    digitalWrite(CLOCK_PIN, HIGH);
    digitalWrite(CLOCK_PIN, LOW);
  }

  data[2] ^= 0x80;
  return ((uint32_t) data[2] << 16) | ((uint32_t) data[1] << 8) | (uint32_t) data[0];
}
