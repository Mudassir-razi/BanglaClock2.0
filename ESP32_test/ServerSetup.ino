//.....................................................................................................
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
//const char* PARAM_INPUT_3 = "ip";
//const char* PARAM_INPUT_4 = "gateway";

//Variables to save values from HTML form
String ssid;
String pass;
//String ip;
//String gateway;

// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
//const char* ipPath = "/ip.txt";
//const char* gatewayPath = "/gateway.txt";


IPAddress localIP;

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Read File from SPIFFS
String readFile(fs::FS& fs, const char* path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while (file.available()) {
    fileContent = file.readStringUntil('\n');
    break;
  }
  return fileContent;
}

// Write file to SPIFFS
void writeFile(fs::FS& fs, const char* path, const char* message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)
// Initialize WiFi
bool initWiFi() {
  if (ssid == "") {
    Serial.println("Undefined SSID ");
    return false;
  }

  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.println("Connecting to WiFi...");

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while (WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println("Failed to connect.");
      return false;
    }
  }

  Serial.println(WiFi.localIP());
  return true;
}
AsyncWebServer Server(80);

void setupServer() {
  initSPIFFS();


  // Load values saved in SPIFFS
  ssid = readFile(SPIFFS, ssidPath);
  pass = readFile(SPIFFS, passPath);
  //ip = readFile(SPIFFS, ipPath);
  //gateway = readFile (SPIFFS, gatewayPath);
  Serial.println(ssid);
  Serial.println(pass);
  // Serial.println(ip);
  //Serial.println(gateway);

  if (initWiFi()) {
    // Route for root / web page
    Server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(SPIFFS, "/index.html", "text/html");
      Serial.println("Got html data");
    });

    //Server.serveStatic("/", SPIFFS, "/");

    Server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(SPIFFS, "/style.css", "text/css");
      Serial.println("Got style data");
    });


    Server.on("/get-data", HTTP_GET, [](AsyncWebServerRequest * request) {
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


    Server.onRequestBody([](AsyncWebServerRequest * request, uint8_t* data, size_t len, size_t index, size_t total) {
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
    Server.begin();
  } else {
    // Connect to Wi-Fi network with SSID and password
    const char* esp_ssid = "ESP_Access";
    const char* esp_pass = "banglaclock20";
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

    Server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(SPIFFS, "/wifiConf.html", "text/html");
      Serial.println("Got WifiConf page");
    });

    Server.on("/style1.css", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(SPIFFS, "/style1.css", "text/css");
      Serial.println("Got style data");
    });

    //Server.serveStatic("/", SPIFFS, "/");

    Server.onRequestBody([](AsyncWebServerRequest * request, uint8_t* data, size_t len, size_t index, size_t total) {
      Serial.println("Running");
      if (request->url() == "/config_wifi") {
        Serial.println("Done receiving data.");
        StaticJsonDocument<100> doc;
        DeserializationError error = deserializeJson(doc, (const char*)data);

        if (!error) {
          const char* newSsid = doc["ssid"];
          const char* newPassword = doc["password"];

          Serial.println("SSID:" + String(newSsid));
          Serial.println("Passwrod: " + String(newPassword));
          // Write file to save value
          writeFile(SPIFFS, ssidPath, newSsid);  // Write file to save value
          writeFile(SPIFFS, passPath, newPassword);
          //conntect to wiffi and send ip address
          WiFi.begin(newSsid, newPassword);
          Serial.println("Connecting to WiFi...");

          unsigned long currentMillis = millis();
          previousMillis = currentMillis;

          while (WiFi.status() != WL_CONNECTED) {
            currentMillis = millis();
            if (currentMillis - previousMillis >= interval) {
              Serial.println("Failed to connect.");
              return false;
            }
          }
          if (WiFi.status() == WL_CONNECTED) {
            String jsonResponse = "{\"ipAddress\":\"" + WiFi.localIP().toString() + "\"}";
            request->send(200, "application/json", jsonResponse);
          }

        } else {
          Serial.println("Failed to parse JSON");
          Serial.println(error.c_str());
          request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
        }
      }
      request->send(200, "text/plain", "end");
      delay(3000);
      ESP.restart();
    });
    Server.begin();
  }
}
