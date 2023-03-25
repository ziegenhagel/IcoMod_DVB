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
#include <chrono>
#include "TextUtils.h"

IcoMod_DVB::IcoMod_DVB(Adafruit_ST7735* tft, unsigned int colors[], JsonObject &config)
{
  // have a private variable to the url string
  _url = "https://webapi.vvo-online.de/dm";

  // get stopid from config
  String stopid = config["stopid"];

  // have the request string
  _payload = "{\"limit\":5,\"stopid\":" + stopid + ",\"isarrival\":false,\"shorttermchanges\":true,\"mentzonly\":false,\"mot\":[\"Tram\",\"CityBus\",\"Cableway\",\"Ferry\"]}";

  // last time the module was refreshed
  _lastRefresh = 0;

  _departures = JsonArray();

  _tft = tft;
  _colors = colors;

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

  // check if the module was refreshed in the last 10 seconds
  if (millis() - _lastRefresh > 10000) {
    IcoMod_DVB::refreshDepartures();

    // clear the screen
    _tft->fillScreen(_colors[0]);

    // TODO: draw the departures
    TextUtils::printRightAligned(_tft, "DVB", 10, 10, 2, _colors[1]);

    TextUtils::printRightAligned(_tft, "Linie      Zeit ", 0, 37 , 1, _colors[1]);

    // foreach departure
    for (int i = 0; i < _departures.size(); i++) {

      Serial.println("Departure: " + String(i));

      // get the departure
      JsonObject departure = _departures[i];
      String lineName = departure["LineName"].as<String>();
      String direction = departure["Direction"].as<String>();
      String time = departure["RealTime"].as<String>();

      // print the line name
      TextUtils::printRightAligned(_tft, lineName + " " +direction +  "   bald ", 0, 54 + (i * 12), 1, _colors[1]);
    }
  }

}


void IcoMod_DVB::refreshDepartures() {
  // set the last refresh time to now
  _lastRefresh = millis();

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
    // Serial.println(payload);

    // create a new json object
    DynamicJsonDocument doc(1024);

    // deserialize the json object
    deserializeJson(doc, payload);

    // get the departures array
    _departures = doc["Departures"];

  } else {
    // print the error
    Serial.println("Error on HTTP request");
  }

  // close the http client
  http.end();
}

// have a helper function to get a human readable time from the date format string
String IcoMod_DVB::getHumanReadableTime(String time) {

  // time looks like this \/Date(1679762460000-0000)\/
  Serial.println(time);

  // remove the first 6 characters
  time = time.substring(6);

  // remove the last 7 characters
  time = time.substring(0, time.length() - 7);

 // convert the time to a long
  long timeLong = atol(time.c_str()); // divide by 1000 to convert from milliseconds to seconds

  // create a new time object
  Serial.println(timeLong);

  // create a new time object
  time_t timeObject = timeLong;

  // create a new time info object
  struct tm * timeinfo;

  // get the time info
  timeinfo = localtime(&timeObject);
  
  // output the complete date and time
  Serial.println(asctime(timeinfo));

  // create a new string
  String timeString;

  // add the hours to the string
  timeString += timeinfo->tm_hour;

  // add the minutes to the string
  timeString += ":";

  // add the minutes to the string
  timeString += timeinfo->tm_min;

  // return the time string
  return timeString;

}

