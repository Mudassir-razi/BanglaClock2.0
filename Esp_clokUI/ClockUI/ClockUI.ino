#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "ArduinoJson.h"
#include "TimeKeeper.h"

const char* esp_ssid = "NoName";
const char* esp_pass = "vallagena";
const char* ssid = "janina";
const char* password = "nijerdatakin" ;


bool apMode = false;
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Check if we should enter AP provisioning mode

  if (!connectToWiFi()) {
    enterAPMode();
  }

  // if wifi is not connected configure wifi here
if(WiFi.status() != WL_CONNECTED )
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/wifiConf.html", "text/html");
  });

  server.on("/style1.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style1.css", "text/css");
    Serial.println("Got style data");
  });

  server.onRequestBody([](AsyncWebServerRequest * request, uint8_t* data, size_t len, size_t index, size_t total) {
    Serial.println("Running");
    if (request->url() == "/config_wifi") {
      Serial.println("Done receiving data.");
      StaticJsonDocument<100> doc1;
      DeserializationError error = deserializeJson(doc1, (const char*)data);

      if (!error) {
        const char* newSsid = doc1["ssid"];
        const char* newPassword = doc1["password"];
        ssid = newSsid;
        password = newPassword;
        Serial.println("SSID:" + String(ssid));
        Serial.println("Passwrod: " + String(password));
        if (configureWiFi(String(ssid), String(password))) {
          // Respond with JSON containing the ESP32's IP address
          String jsonResponse = "{\"ipAddress\":\"" + WiFi.localIP().toString() + "\"}";
          request->send(200, "application/json", jsonResponse);
          delay(10000);
          WiFi.softAPdisconnect(true);
        } else {
          request->send(400, "text/plain", "Wi-Fi configuration failed");
        }

      } else {
        Serial.println("Failed to parse JSON");
        Serial.println(error.c_str());
        request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
      }
    }
    request->send(200, "text/plain", "end");
  });
}
  
  server.on("/192.168.0.120" , HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
    Serial.println("Got style data");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
    Serial.println("Got style data");
  });


  server.on("/get-data", HTTP_GET, [](AsyncWebServerRequest * request) {
    TimeKeeper time;
    Serial.println("Received Ntp data:");
    Serial.println("Hour: " + String(time.getHour()));
    Serial.println("Minute: " + String(time.getMin()));
    Serial.println("Second: " + String(time.getSec()));
    Serial.println("Day: " + String(time.getDay()));
    Serial.println("Month: " + String(time.getMonth()));
    Serial.println("Year: " + String(time.getYear()));
    Serial.println("Temperature: " + String(time.getTemp()));
    Serial.println("Humidity: " + String(time.getHumidity()));

    // Create a JSON response with the sensor data
    StaticJsonDocument<256> jsonResponse;
    jsonResponse["hour"] = time.getHour();
    jsonResponse["minute"] = time.getMin();
    jsonResponse["second"] = time.getSec();
    jsonResponse["day"] = time.getDay();
    jsonResponse["month"] = time.getMonth();
    jsonResponse["year"] = time.getYear();
    jsonResponse["humidity"] = time.getHumidity();
    jsonResponse["temperature"] = time.getTemp();

    String jsonData;
    serializeJson(jsonResponse, jsonData);

    // Send the JSON response to the client
    request->send(200, "application/json", jsonData);
  });


  server.onRequestBody([](AsyncWebServerRequest * request, uint8_t* data, size_t len, size_t index, size_t total) {
    Serial.println("Running");
    if (request->url() == "/update-time") {
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, (const char*)data);

      if (!error) {
        const char* hour = doc["hour"];
        const char* minute = doc["minute"];
        const char* second = doc["second"];
        const char* day = doc["day"];
        const char* month = doc["month"];
        const char* year = doc["year"];
        const char* dayName = doc["dayName"];
        Serial.println("Received JSON data:");
        Serial.println("Hour: " + String(hour));
        Serial.println("Minute: " + String(minute));
        Serial.println("Second: " + String(second));
        Serial.println("Day: " + String(day));
        Serial.println("Month: " + String(month));
        Serial.println("Year: " + String(year));
        Serial.println("DayName: " + String(dayName));

        // Use the received time values (hour, minute, second, day, month, year, dayName) to update your ESP32's clock
        // Implement your code here to handle time updates
      } else {
        Serial.println("Failed to parse JSON");
        Serial.println(error.c_str());
        request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
      }
    }
    request->send(200, "text/plain", "end");
  });



  server.begin();
}

void loop() {

}

bool connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");

  // Try to connect to Wi-Fi
  int timeout = 30; // Adjust the timeout as needed
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(1000);
    Serial.print(".");
    timeout--;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    return false;
  }
}

void enterAPMode() {
  apMode = true;
  // Start an access point with a specific SSID and password
  WiFi.softAP(esp_ssid, esp_pass);
  Serial.print("Connecting to Esp hotspot..");
  while (!WiFi.softAPgetStationNum()) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected");
  Serial.println("No of device connected: " + String(WiFi.softAPgetStationNum()));

  IPAddress apIP(192, 168, 1, 1);
  Serial.println();
  Serial.print("Esp Ip address: ");
  Serial.println(apIP);

  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  Serial.println("Entered AP provisioning mode.");
}

bool configureWiFi(const String& newSsid, const String& newPassword) {
  if (newSsid.length() > 0 && newPassword.length() >= 8) {
    WiFi.begin(newSsid.c_str(), newPassword.c_str());
    Serial.print("Configuring Wi-Fi with SSID: ");
    Serial.println(newSsid);

    // Wait for the ESP32 to connect to Wi-Fi
    int timeout = 30; // Adjust the timeout as needed
    Serial.print("Connecting to");
    Serial.print(newSsid);
    Serial.print("..");
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
      delay(1000);
      Serial.print(".");
      timeout--;
    }
    Serial.print("Connected");
    Serial.println("");

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      apMode = false; // Exit AP provisioning mode
      return true;
    }
  }
  return false;
}
