//This example shows how to use different fonts on a VGA screen.
//You need to connect a VGA screen cable to the pins specified below.
//cc by-sa 4.0 license
//bitluni

#include "ESP32Lib.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Ressources/Font6x8.h>
#include <Ressources/CustomBangla.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include "TimeKeeper.h"
#include <time.h>

struct weatherInfo {
  double temp;
  double hum;
  double feel;
};

//wifi stuff
//const char* ssid = "Janina";
//const char* password = "nijerdatakin";

//pin configuration
const int redPin = 14;
const int greenPin = 19;
const int bluePin = 27;
const int hsyncPin = 32;
const int vsyncPin = 33;

//VGA Device
VGA3Bit vga;

//.............................................................................
//timer
TimeKeeper time_keeper;
struct tm dateTime;
struct weatherInfo wi;
int timeMode = 1;      //0 for 24hrs system
int infoLastUpdated;
int screenLastUpdated; 
int deltaTime;
bool feelEn;

//print characters
char clock_[5];
char date_[30];
char temp_[15];
char hum_[15];
char feel_[10];
char wday_[10];

void strcpy(char *dest, char *source)
{
  while(*source)
  {
    *dest = *source;
    source++;
    dest++;
  }  
}

void VGA_Setup() {
  //initializing vga at the specified pins
  vga.init(vga.MODE400x300, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
  vga.setTextColor(vga.RGB(0, 255, 255), vga.RGB(0, 0, 0));
  //vga.setFont(Font6x8);
  vga.setCustomFont(customBangla);
}

//updates time through http
void updateTimeOnline() {

  /*
  Serial.printf("Connecting to %s ", ssid);
  infoLastUpdated = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(100);
  }
  */

  if (WiFi.status() != WL_CONNECTED) Serial.println("Connection time out");
  else Serial.println("Connected");

  dateTime.tm_year = time_keeper.getYear();
  dateTime.tm_mon = time_keeper.getMonth();
  dateTime.tm_mday = time_keeper.getDay();
  dateTime.tm_wday = time_keeper.getWeekday();
  dateTime.tm_hour = time_keeper.getHour();
  dateTime.tm_min = time_keeper.getMin();

  //updating weather
  wi.temp = time_keeper.getTemp();
  wi.hum = time_keeper.getHumidity();
  wi.feel = time_keeper.getFeelslike();
  delay(3000);
  //WiFi.disconnect();
}


//updats time using local RTS
void updateTimeOffline() {

  if(dateTime.tm_min != time_keeper.getMin())
  {
    dateTime.tm_min = time_keeper.getMin();
    vga.clear();
  }

  dateTime.tm_hour = time_keeper.getHour();
  dateTime.tm_min = time_keeper.getMin();
}

//translates & formats time and weather info for sending to VGA
void formatTime()
{
  if(timeMode == 1)
  {
    dateTime.tm_hour = dateTime.tm_hour % 12;
    dateTime.tm_hour = !dateTime.tm_hour ? 12 : dateTime.tm_hour;
  }

  char hr[4];
  char min[4];
  if(dateTime.tm_hour >= 0 && dateTime.tm_hour < 10)sprintf(hr, "0%d", dateTime.tm_hour);
  else sprintf(hr, "%d", dateTime.tm_hour);
  if(dateTime.tm_min >= 0 && dateTime.tm_min < 10)sprintf(min, "0%d", dateTime.tm_min);
  else sprintf(min, "%d", dateTime.tm_min);
  sprintf(clock_, "%s { %s", hr, min);

  //update date
  char monName[8];
  switch(dateTime.tm_mon + 1)
  {
    case 1:
      strcpy(monName, "iQY");
      break;
    case 2:
      strcpy(monName, "jq");
      break;
    case 3:
      strcpy(monName, "AXR");
      break;
    case 4:
      strcpy(monName, "Wbn");
      break;
    case 5:
      strcpy(monName, "Vy");
      break;
    case 6:
      strcpy(monName, "AFj");
      break;
    case 7:
      strcpy(monName, "KLk");
      break;
    case 8:
      strcpy(monName, "oGHen");
      break;
    case 9:
      strcpy(monName, "wx");
      break;
    case 10:
      strcpy(monName, "Mz");
      break;
    case 11:
      strcpy(monName, "fgj");
      break;
    case 12:
      strcpy(monName, "cO");
      break;
  }
  sprintf(date_, "%d %s %d", dateTime.tm_mday, monName, dateTime.tm_year);

  //update week day
  switch(dateTime.tm_wday)
  {
    case 0:
      strcpy(monName, "rIBr");
      break;
    case 1:
      strcpy(monName, "SmBr");
      break;
    case 2:
      strcpy(monName, "m@lBr");
      break;
    case 3:
      strcpy(monName, "UdBr");
      break;
    case 4:
      strcpy(monName, "vhPTBr");
      break;
    case 5:
      strcpy(monName, "uCBr");
      break;
    case 6:
      strcpy(monName, "sNBr");
      break;
  }
  sprintf(wday_, "%s", monName);


  //temperature
  sprintf(temp_, "tpMa{ %.0lf}", wi.temp-273);

  //humidity
  sprintf(hum_, "Ayt{ %.0lf%%", wi.hum);

  //feels like:
  sprintf(feel_, "oZ!T{ %.0lf}", wi.feel - 273);

}

void setup() {
  Serial.begin(115200);

  setupServer();

  delay(1000);
  deltaTime = 0;
  screenLastUpdated = 0;
  infoLastUpdated = 0;
  feelEn = false;

  updateTimeOnline();
  VGA_Setup();
}

void loop() {
  int initTime = millis(); 

  screenLastUpdated += deltaTime;
  infoLastUpdated += deltaTime;

  if (infoLastUpdated > 1000 * 3600) updateTimeOnline();
  else updateTimeOffline();

  if(screenLastUpdated > 1000 * 3)
  {
    vga.clear();
    screenLastUpdated = 0;
  }

  formatTime();
  //Serial.println(time_keeper.getHumidity());

  //Time part
  vga.textScale = 3;
  vga.cursorX = vga.cursorBaseX + 40;
  vga.cursorY = 0;
  vga.printlnCustom(clock_);

  //Date part
  vga.cursorY = 210;
  vga.cursorX = vga.cursorBaseX;
  vga.textScale = 1;
  vga.printlnCustom(date_);

  //week day part
  vga.cursorY = 240;
  vga.cursorX = vga.cursorBaseX;
  vga.textScale = 1;
  vga.printlnCustom(wday_);

  //temp info
  vga.cursorX = vga.cursorBaseX + 200;
  vga.cursorY = 210;
  vga.textScale = 1;
  vga.printlnCustom(temp_);

  //humidity info
  vga.cursorX = vga.cursorBaseX + 200;
  vga.cursorY = 240;
  vga.textScale = 1;
  vga.printlnCustom(hum_);


  //update delta time
  deltaTime = millis() - initTime;
  
}
