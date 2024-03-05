#ifndef MULTI_HX711_h
#define MULTI_HX711_h

#if defined(ESP8266) || defined(ESP32)
  #define DELAY_FOR_FAST_MCU delayMicroseconds(1);
#else
  #define DELAY_FOR_FAST_MCU
#endif

#define BEGIN_ATOMIC_BLOCK noInterrupts();
#define END_ATOMIC_BLOCK interrupts();


#include "Arduino.h"

class MULTI_HX711
{
  private:
    byte* CLOCK_PINS; // Ändern auf Array-Zeiger
    byte* OUT_PINS; // Ändern auf Array-Zeiger
    uint16_t* FACTOR; // Privates Mitglied für das Faktorarray
    uint32_t* data; // Privates Mitglied für das Datenarray
    uint32_t* tare; // Privates Mitglied für das Taraarray


    byte GAIN;
    byte num_out; // Anzahl der Outpins
    byte num_clk; // Anzahl der Clockpins
    void init(byte* output_pins, byte* clock_pins, byte num_out, byte num_clk); // Deklaration der init() Methode
    
  public:
    // Überladener Konstruktor für einzelne Pins
    MULTI_HX711(byte output_pin, byte clock_pin);
    
    // Überladener Konstruktor für Arrays bekannter Länge
    MULTI_HX711(byte* output_pins, byte* clock_pins, byte num_out, byte num_clk);
    
    virtual ~MULTI_HX711();
    
    bool readyToSend();
    void setGain(byte gain = 128);
    uint32_t* read();
    uint32_t* readTare();
    uint32_t* readTareKilo();
    void setTare(byte runs, byte delays);
    void setFaktor(uint16_t* factor);
};

#endif /* MULTI_HX711_h */