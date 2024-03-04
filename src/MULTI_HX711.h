#ifndef MULTI_HX711_h
#define MULTI_HX711_h

#include "Arduino.h"

class MULTI_HX711
{
  private:
    byte* CLOCK_PINS; // Ändern auf Array-Zeiger
    byte* OUT_PINS; // Ändern auf Array-Zeiger
    byte GAIN;
    byte num_out; // Anzahl der Outpins
    byte num_clk; // Anzahl der Clockpins
    bool pinsConfigured;
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
};

#endif /* MULTI_HX711_h */