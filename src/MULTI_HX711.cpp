#include <Arduino.h>
#include "MULTI_HX711.h"

MULTI_HX711::MULTI_HX711(byte output_pin, byte clock_pin)
{
  byte clock_pins[] = {clock_pin};  // Erzeuge ein Array mit einem Element
  byte out_pins[] = {output_pin};   // Erzeuge ein Array mit einem Element
  init(out_pins, clock_pins, 1, 1); // Rufe die init-Methode mit den Arrays der Länge 1 auf
}

MULTI_HX711::MULTI_HX711(byte *output_pins, byte *clock_pins, byte num_out, byte num_clk)
{
  init(output_pins, clock_pins, num_out, num_clk); // Rufe die init-Methode mit den übergebenen Arrays auf
}

void MULTI_HX711::init(byte *output_pins, byte *clock_pins, byte num_out, byte num_clk)
{
  // Initialisierung der Variablen
  GAIN = 1;

  // Speichere die Anzahl der Outpins und Clockpins
  this->num_out = num_out;
  this->num_clk = num_clk;

  // Allokiere Speicher für die Arrays und kopiere die Werte
  CLOCK_PINS = new byte[num_clk];
  OUT_PINS = new byte[num_out];
  data = new uint32_t[num_out]; // erhält seine werte in this->read()
  tare = new uint32_t[num_out]; // erhält leere werte bis tariert wurde

  for (byte i = 0; i < num_clk; i++)
  {
    CLOCK_PINS[i] = clock_pins[i];
  }
  for (byte i = 0; i < num_out; i++)
  {
    OUT_PINS[i] = output_pins[i];
  }
  for (byte i = 0; i < num_out; i++)
  {
    tare[i] = 0;
  }
  // Konfiguriere die Pins, falls noch nicht geschehen
  for (byte i = 0; i < num_clk; i++)
  {
    pinMode(CLOCK_PINS[i], OUTPUT);
  }
  for (byte i = 0; i < num_out; i++)
  {
    pinMode(OUT_PINS[i], INPUT);
  }
}

MULTI_HX711::~MULTI_HX711()
{
  // Freigabe des allokierten Speichers für die Arrays
  delete[] CLOCK_PINS;
  delete[] OUT_PINS;
  delete[] data;
  delete[] tare;
}

void MULTI_HX711::setTare(byte runs, byte delays) {
  // Setze die Tare-Werte auf Null, bevor neue Messungen durchgeführt werden
  for (byte j = 0; j < num_out; j++) {
    tare[j] = 0;
  }
  // Führe Tara-Messungen durch und aktualisiere die Tare-Werte entsprechend
  for (byte i = 0; i < runs; i++) {
    // Lese die Messwerte und speichere sie im Datenarray
    read();
    // Addiere die aktuellen Messwerte zu den Tare-Werten
    for (byte j = 0; j < num_out; j++) {
      tare[j] += data[j] / runs;
    }
    // Verzögerung zwischen den einzelnen Messungen
    delay(delays);
  }
}

bool MULTI_HX711::readyToSend()
{
  // Iteriere durch alle Output-Pins
  for (byte i = 0; i < num_out; i++)
  {
    // Prüfe, ob der aktuelle Output-Pin nicht bereit ist
    if (digitalRead(OUT_PINS[i]) != LOW)
    {
      return false; // Wenn nicht bereit, gebe false zurück
    }
  }
  return true; // Wenn alle bereit sind, gebe true zurück
}

void MULTI_HX711::setGain(byte gain)
{
  switch (gain)
  {
  case 128:
    GAIN = 1;
    break;
  case 64:
    GAIN = 3;
    break;
  case 32:
    GAIN = 2;
    break;
  default:
    // Wenn ein ungültiger Wert übergeben wird, setze GAIN auf den Standardwert (1)
    GAIN = 1;
    break;
  }

  // Iteriere durch alle Clock-Pins und setze sie auf LOW
  for (byte i = 0; i < num_clk; i++)
  {
    digitalWrite(CLOCK_PINS[i], LOW);
  }

  // Führe die read()-Methode aus
  read();
}

uint32_t *MULTI_HX711::read()
{
  while (!readyToSend())
    ;
  // leere jedes mal das Ergebnisarray
  for (byte j = 0; j < this->num_out; j++)
    data[j] = 0;

  BEGIN_ATOMIC_BLOCK;

  for (int i = 0; i < (GAIN + 24); i++)
  {
    DELAY_FOR_FAST_MCU; // Optional: Wartezeit für schnelle Mikrocontroller
    // Schalte alle Clock-Pins gleichzeitig um
    for (byte j = 0; j < this->num_clk; j++)
    {
      digitalWrite(CLOCK_PINS[j], HIGH);
    }
    DELAY_FOR_FAST_MCU; // Optional: Wartezeit für schnelle Mikrocontroller
    for (byte j = 0; j < this->num_clk; j++)
    {
      digitalWrite(CLOCK_PINS[j], LOW);
    }

    // Lies die Daten von den Input-Pins und speichere sie im entsprechenden Datenarray
    if (i < 24)
    {
      for (byte j = 0; j < this->num_out; j++)
      {
        data[j] = (data[j] << 1) | digitalRead(OUT_PINS[j]);
      }
    }
  }

  END_ATOMIC_BLOCK;
  // Flippe Bit 23 in jedem Datenwert
  for (byte j = 0; j < num_out; j++)
  {
    data[j] ^= 0x800000;
  }

  return data;
}

uint32_t *MULTI_HX711::readTare(){
  read();
  for (byte j = 0; j < num_out; j++)
  {
    data[j] -= tare[j];
  }
  return data;
}
