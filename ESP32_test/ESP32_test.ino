#include <WiFi.h>
#include "time.h"
#include "sntp.h"

struct tm timeInfo;

//status flags
bool connected;
bool timeReceived;


const char* ssid       = "Janina";
const char* password   = "nijerdatakin";


void ConnectToWifi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  connected = true;
}

void setup()
{
  connected = false;
  timeReceived = false;

  Serial.begin(115200);
  //configureTimeKeeper();
  ConnectToWifi();
}

int i = 0;
void loop()
{
  UpdateTime();
  delay(100);
  Serial.println("Hello world");
  i++;
  if(i > 50 && connected)
  {
    Serial.println("Disconnected............................");
    connected = false;
    WiFi.disconnect();
  }
}