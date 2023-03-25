/*
  IcoMod_Logo.cpp - IcoDesk Module to display the animated Logo.
  Created by Till Michels, February 2, 2023.
*/

#ifndef IcoMod_Logo_h
#define IcoMod_Logo_h

#include "Arduino.h"
#include "IcoMod.h"
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

class IcoMod_DVB : public IcoMod
{
  public:
    IcoMod_DVB(Adafruit_ST7735* tft, unsigned int colors[], JsonObject &config);
    void onClick();
    void initialize();
    void refresh();
};

#endif