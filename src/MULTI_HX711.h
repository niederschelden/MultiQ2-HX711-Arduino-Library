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
    int32_t* data; // Privates Mitglied für das Datenarray
    float* kilos; // Privates Mitglied für das Kiloarray
    int32_t* tare; // Privates Mitglied für das Taraarray

    bool BLOCK = false;
    byte GAIN;
    byte num_out; // Anzahl der Outpins
    byte num_clk; // Anzahl der Clockpins
    
    
  public:
    //Konstruktor
    MULTI_HX711();
    //Destruktor
    virtual ~MULTI_HX711();
    //Methoden
    void init(byte output_pin, byte clock_pin);
    void init(byte* output_pins, byte* clock_pins, byte num_out, byte num_clk); //MUSS aufgerufen werden
    bool readyToSend();
    void setGain(byte gain = 128);
    int32_t* read();
    int32_t* readTare();
    float* readTareKilo();
    void setTare(byte runs, byte delays);
    void setFactor(uint16_t factor);
    void setFactor(uint16_t* factor);
    byte getNumOut() {
        return num_out;
        }
};

#endif /* MULTI_HX711_h */