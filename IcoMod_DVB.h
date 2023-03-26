/*
  IcoMod_DVB.cpp - IcoDesk Module to display the animated Logo.
  Created by Till Michels, February 2, 2023.
*/

#ifndef IcoMod_DVB_h
#define IcoMod_DVB_h

#include "Arduino.h"
#include "IcoMod.h"
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <chrono>

class IcoMod_DVB : public IcoMod
{
  public:
    IcoMod_DVB(Adafruit_ST7735* tft, unsigned int colors[], JsonObject &config);
    void onClick();
    void initialize();
    void refresh();
    void refreshDepartures();
    String getHumanReadableTime(String time);
    void getTimestamp();

  private:
    String _url;
    String _payload;
    unsigned long _lastRefresh;
    JsonArray _departures;
    String _stopName;
    JsonArray _stopIds;
    unsigned int _currentStopId;
    Adafruit_ST7735* _tft;
    unsigned int* _colors;
    unsigned long long _currentTimestamp;
};

#endif