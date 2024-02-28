#include <AsyncTCP.h>
#include <ESP32Time.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <Wire.h>

#include "SPIFFS.h"
#include "time.h"

const char *ssid = "EscapeFactoryWireless";
const char *password = "T1m3T03$c@p3";
String hostname = "clockclock";

// Time Variables
int currentHour = 0;
int currentMinute = 0;

// Auto fetch time
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 0;
int previousDaySync = 0;

ESP32Time rtc(daylightOffset_sec);
struct tm timeinfo;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String &var) {
  if (var == "TIME") {
    return (String(rtc.getHour(true)) + ":" + String(rtc.getMinute()) + ":" +
            String(rtc.getSecond()));
  }
  return String();
}

void fetchTimeOnline() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  if (getLocalTime(&timeinfo)) {
    rtc.setTimeStruct(timeinfo);
  }

  currentHour = rtc.getHour();
  currentMinute = rtc.getMinute();

  Serial.println(rtc.getHour());
  Serial.println(rtc.getMinute());
  Serial.println(rtc.getSecond());

  previousDaySync = rtc.getDay();
}

void setup() {
  Serial.begin(115200);

  Wire.begin();

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Start server
  server.begin();

  fetchTimeOnline();
}

void sendTimeToClockSlave() {}

void loop() {
  if (rtc.getDay() != previousDaySync) {
    // if a day has passed sync the time with the servers again
    fetchTimeOnline();
  }

  if (currentHour != rtc.getHour()) {
    // if the previous hour doesn't equal the current hour on display anymore
    Serial.println("next hour.");
    currentHour = rtc.getHour();
  }

  if (currentMinute != rtc.getMinute()) {
    // if the previous minute doesn't equal the current minute on display
    Serial.println("next minute.");
    currentMinute = rtc.getMinute();
  }
}
