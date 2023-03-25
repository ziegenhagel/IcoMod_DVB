/*
  IcoMod_Logo.cpp - IcoDesk Module to display the animated Logo.
  Created by Domenic Böse, February 2, 2023.
*/

#include "Arduino.h"
#include "IcoMod_DVB.h"
#include "ArduinoJson.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "RandomUtils.h"

IcoMod_DVB::IcoMod_DVB(Adafruit_ST7735* tft, unsigned int colors[], JsonObject &config)
{
}

void IcoMod_DVB::onClick()
{
  // print that the module is clicked
  Serial.println("DVB Module clicked");
}

void IcoMod_DVB::initialize()
{

  // print that the module is initialized
  Serial.println("DVB Module initialized");
 
}

void IcoMod_DVB::refresh()
{
  // print that the module is refreshed
  Serial.println("DVB Module refreshed");
}