/*
  IcoMod_DVB.cpp - IcoDesk Module to display the animated Logo.
  Created by Domenic Böse, February 2, 2023.
*/

#include "Arduino.h"
#include "IcoMod_DVB.h"
#include "ArduinoJson.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <HTTPClient.h>

IcoMod_DVB::IcoMod_DVB(Adafruit_ST7735* tft, unsigned int colors[], JsonObject &config)
{
  // have a private variable to the url string
  _url = "https://webapi.vvo-online.de/dm";

  // get stopid from config
  String stopid = config["stopid"];

  // have the request string
  _payload = '{"limit":5,"stopid":' + stopid + ',"isarrival":false,"shorttermchanges":true,"mentzonly":false,"mot":["Tram","CityBus","Cableway","Ferry"]}';

  // last time the module was refreshed
  _lastRefresh = 0;

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

  // check if the module was refreshed in the last 10 seconds
  if (millis() - _lastRefresh > 10000) {
    IcoMod_DVB::refreshDepartures();
  }

  // TODO: draw the departures

}


void IcoMod_DVB::refreshDepartures() {
    // set the last refresh time to now
    _lastRefresh = millis();

    // print the request string
    Serial.println(_payload);

    // create a new http client
    HTTPClient http;

    // set the url
    http.begin(_url);

    // set the content type
    http.addHeader("Content-Type", "application/json");

    // set the request method to POST
    int httpCode = http.POST(_payload);

    // check if the request was successful
    if (httpCode > 0) {
      // print the http code
      Serial.println(httpCode);

      // get the response
      String payload = http.getString();

      // print the response
      Serial.println(payload);

      // create a new json object
      DynamicJsonDocument doc(1024);

      // deserialize the json object
      deserializeJson(doc, payload);

      // get the departures array
      JsonArray departures = doc["Departures"];

      // print the first departure
      Serial.println(departures[0]["LineName"].as<String>());
    } else {
      // print the error
      Serial.println("Error on HTTP request");
    }

    // close the http client
    http.end();
  }

}