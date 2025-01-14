#include <Arduino.h>
#include "MULTI_HX711.h"

//Dummy Konstruktor
MULTI_HX711::MULTI_HX711(){}

//Alte init Methode für nur ein HX711
void MULTI_HX711::init(byte output_pin, byte clock_pin)
{
  byte clock_pins[] = {clock_pin};  // Erzeuge ein Array mit einem Element
  byte out_pins[] = {output_pin};   // Erzeuge ein Array mit einem Element
  init(out_pins, clock_pins, 1, 1); // Rufe die init-Methode mit den Arrays der Länge 1 auf
}

//Neue init Methode für mehrere HX711
void MULTI_HX711::init(byte *output_pins, byte *clock_pins, byte num_out, byte num_clk)
{
  // Initialisierung der Variablen
  setGain(128);

  // Speichere die Anzahl der Outpins und Clockpins
  this->num_out = num_out;
  this->num_clk = num_clk;

  // Allokiere Speicher für die Arrays und kopiere die Werte
  CLOCK_PINS = new byte[num_clk];
  for (byte i = 0; i < num_clk; i++) CLOCK_PINS[i] = clock_pins[i];

  OUT_PINS = new byte[num_out];
  for (byte i = 0; i < num_out; i++) OUT_PINS[i] = output_pins[i];

  FACTOR = new uint16_t[num_out]; 
  for (byte i = 0; i < num_out; i++) FACTOR[i] = 0;
 
  tare = new int32_t[num_out]; 
  for (byte i = 0; i < num_out; i++) tare[i] = 0;

  kilos = new float[num_out]; 
  for (byte i = 0; i < num_out; i++) kilos[i] = 0.0;

  data = new int32_t[num_out]; // erhält seine werte in this->read()

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
  delete[] kilos; 
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
  if (BLOCK) return false;
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

int32_t *MULTI_HX711::read()
{
    // leere jedes mal das Ergebnisarray
  for (byte j = 0; j < this->num_out; j++)
    data[j] = 0;
    
  while (!readyToSend());
  BLOCK = true;
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
  BLOCK = false;
  END_ATOMIC_BLOCK;
  // Flippe Bit 23 in jedem Datenwert
  for (byte j = 0; j < num_out; j++)
  {
    data[j] ^= 0x800000;
  }
  //Gib die Refernz auf den Speicherort zurück
  return data;
}

int32_t *MULTI_HX711::readTare(){
  read();
  for (byte j = 0; j < num_out; j++)
  {
     data[j] -= tare[j];
  }
  return data;
}

float* MULTI_HX711::readTareKilo() {
    readTare();
    for (byte j = 0; j < num_out; j++) {
        if (FACTOR[j] != 0) { // Faktor überprüfen
            kilos[j] = static_cast<float>(data[j]) / FACTOR[j];
        } else {
            kilos[j] = 0.0; // Setze auf 0.0, wenn der Faktor ungültig ist
        }
    }
    return kilos;
}

int32_t* MULTI_HX711::readTareFactorDecimal(byte decimals) {
    readTare(); // Werte mit Tare anpassen

    int32_t scalingFactor = pow(10, decimals); // Skalierungsfaktor berechnen (10^decimals)

    for (byte j = 0; j < num_out; j++) {
        if (FACTOR[j] != 0) { // Faktor überprüfen, um Division durch 0 zu vermeiden
            data[j] = (data[j] * scalingFactor) / FACTOR[j];
        } else {
            data[j] = 0; // Setze auf 0, wenn der Faktor ungültig ist
        }
    }

    return data; // Skalierte Werte zurückgeben
}


//Individuelle Faktoren
void MULTI_HX711::setFactor(uint16_t* factor){
  for (byte j = 0; j < num_out; j++) FACTOR[j]= factor[j]; 
}
//Ein Factor für alle
void MULTI_HX711::setFactor(uint16_t factor){
  for (byte j = 0; j < num_out; j++) FACTOR[j] = factor; 
}
