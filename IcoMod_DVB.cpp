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
#include "TextUtils.h"
#include <iostream>
#include <ctime>

IcoMod_DVB::IcoMod_DVB(Adafruit_ST7735* tft, unsigned int colors[], JsonObject &config)
{
  // have a private variable to the url string
  _url = "https://webapi.vvo-online.de/dm";

  // get stopid from config
  String stopid = config["stopid"];

  // have the request string
  _payload = "{\"limit\":8,\"stopid\":" + stopid + ",\"isarrival\":false,\"shorttermchanges\":true,\"mentzonly\":false,\"mot\":[\"Tram\",\"CityBus\",\"Cableway\",\"Ferry\"]}";

  // last time the module was refreshed
  _lastRefresh = 0;

  _departures = JsonArray();

  _tft = tft;
  _colors = colors;

  _currentTimestamp = 0;


  // get the current timestamp
  IcoMod_DVB::getTimestamp();

  // refresh the departures
  IcoMod_DVB::refreshDepartures();
}

// lets get the current timestamp from http://10.10.10.125:3000/api/timestamp
void IcoMod_DVB::getTimestamp() {
  // create a new http client
  HTTPClient http;

  // set the url
  http.begin("http://10.10.10.125:3000/api/timestamp");

  // set the request method to GET
  int httpCode = http.GET();

  // check if the request was successful
  if (httpCode > 0) {
    // get the payload
    String payload = http.getString();

    // parse the payload
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    // get the timestamp
    _currentTimestamp = doc["timestamp"].as<unsigned long long>();
  }

  // close the connection
  http.end();

  // print the current timestamp

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
    TextUtils::printRightAligned(_tft, "DVB", 10, 10, 2, _colors[2]);
    TextUtils::printRightAligned(_tft, "Zeit ", 0, 37, 1, _colors[1]);

    _tft->setCursor(5, 37);
    _tft->print("Linie");


    // foreach departure
    for (int i = 0; i < _departures.size(); i++) {

      // get the departure
      JsonObject departure = _departures[i];

      String lineName = departure["LineName"].as<String>();

      String direction = departure["Direction"].as<String>();

      // replace all ä/ö/ü with ae/oe/ue and ß with ss and Ä/Ö/Ü with Ae/Oe/Ue
      direction.replace("ä", "ae");
      direction.replace("ö", "oe");
      direction.replace("ü", "ue");
      direction.replace("ß", "ss");
      direction.replace("Ä", "Ae");
      direction.replace("Ö", "Oe");
      direction.replace("Ü", "Ue");

      String time = IcoMod_DVB::getHumanReadableTime(departure["RealTime"].as<String>());


      //TextUtils::printLeftAligned(_tft, " "+ lineName + " " +direction, 0, 54 + (i * 12), 1, _colors[1]); 
      _tft->setCursor(5, 54 + (i * 12));
      _tft->print(lineName);
      _tft->setCursor(20, 54 + (i * 12));
      _tft->print(direction);

      // print the line name
      TextUtils::printRightAligned(_tft,  time+" ", 0, 54 + (i * 12), 1, _colors[1]);

      // draw bitmap
      // _tft->drawBitmap(0, 54 + (i * 12), _bitmap, 10, 10, _colors[1]);
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

    // get the response
    String payload = http.getString();

    // print the response
    // Serial.println(payload);

    // create a new json object
    DynamicJsonDocument doc(1024*12);

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

  time = time.substring(6);
  String unix_timestamp_string = time.substring(0, time.length() - 10);

  // convert the string to a long
  unsigned long long timeLong = unix_timestamp_string.toInt();

  // current time is the current timestamp + millis()
  unsigned long long unix_timestamp_now = (_currentTimestamp + millis()) / 1000;

  // difference between the current time and the departure time
  unsigned long long difference = timeLong - unix_timestamp_now;
  
  // calculate difference in minutes and round (up) minutes
  unsigned long long minutes = (difference / 60) + 1;

  if (minutes > 10000) {
    return "jetzt";
  }


  return String(minutes) + "min";

}

