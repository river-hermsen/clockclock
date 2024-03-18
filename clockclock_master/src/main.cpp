#include <AsyncTCP.h>
#include <ESP32Time.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <WiFi.h>
#include <Wire.h>

#include "SPIFFS.h"
#include "time.h"

// ! Preferences
Preferences preferences;

// ! WIFI
String hostname = "clockclock";
unsigned long previousMillis = 0;
const long timeOutWifi = 10000;

// ! TIME VARIABLES
int currentHour = 0;
int currentMinute = 0;
// Auto fetch time
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 0;
int previousDaySync = 0;

ESP32Time rtc(daylightOffset_sec);
struct tm timeinfo;

// ! WEBSERVER
AsyncWebServer server(80);
// Replaces html placeholder with current time
String processor(const String &var) {
  if (var == "TIME") {
    return (String(rtc.getHour(true)) + ":" + String(rtc.getMinute()) + ":" +
            String(rtc.getSecond()));
  } else if (var == "SSID") {
  }
  return String();
}

void makeSoftAP() {
  Serial.println("CREATING ACCESS POINT...");
  WiFi.softAP("ClockClock", "clockclock");

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}

void tryConnToWifi() {
  preferences.begin("wifiCreds", false);

  String ssidPref = preferences.getString("ssid", "");
  String paswdPref = preferences.getString("password", "");

  if (ssidPref == "" || paswdPref == "") {
    Serial.println("No values saved for ssid or password");
  } else {
    // Try to connect to wifi if creds are found in prefs
    const char *ssid = ssidPref.c_str();
    const char *password = paswdPref.c_str();
    WiFi.begin(ssid, password);
    Serial.println("Trying to connect to WiFi with SSID: " + String(ssid));
    while (WiFi.status() != WL_CONNECTED) {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= timeOutWifi) {
        // Not connected to wifi, creating AP
        Serial.println("NO ABLE TO CONNECT TO NETWORK");
        makeSoftAP();
        return;
      }
    }
    Serial.println("Connected to WiFi with SSID: " + String(ssid));
    Serial.print("IP is: ");
    Serial.println(WiFi.localIP());
  }
  preferences.end();
}

void writeCredToPrefs(const char *ssid, const char *passwd) {
  preferences.begin("wifiCreds", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", passwd);
  preferences.end();
}

void fetchTimeOnline() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  if (getLocalTime(&timeinfo)) {
    rtc.setTimeStruct(timeinfo);
  }

  currentHour = rtc.getHour();
  currentMinute = rtc.getMinute();

  previousDaySync = rtc.getDay();
}

void setup() {
  Serial.begin(115200);
  // I2C
  Wire.begin();

  // INIT PREF
  // writeCredToPrefs("EscapeFactoryWirelesss", "T1m3T03$c@p3");
  tryConnToWifi();

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Fetch time from timeserver again
  server.on("/refreshTime", HTTP_POST, [](AsyncWebServerRequest *request) {
    fetchTimeOnline();
    request->send(200, "text/plain", "Refreshing...");
  });

  // New WiFi Settings
  server.on("/submitWifi", HTTP_POST, [](AsyncWebServerRequest *request) {
    String userSsid;
    String userPasswd;
    userSsid = request->getParam("ssid", true)->value();
    userPasswd = request->getParam("passwd", true)->value();
    Serial.println(userSsid);
    Serial.println(userPasswd);
    writeCredToPrefs(userSsid.c_str(), userPasswd.c_str());
    delay(1000);
    ESP.restart();
    request->send(200, "text/plain", "Rebooting... ");
  });

  // Start server
  server.begin();

  fetchTimeOnline();
}

void sendMinToClockSlave() {
  Serial.printf("Sending minute to clock slave: %i \n", rtc.getMinute());
  Wire.beginTransmission(4);

  uint8_t Buffer[2];
  Buffer[0] = 109;  // 109 is char -> m
  Buffer[1] = rtc.getMinute();
  Wire.write(Buffer, 2);

  Wire.endTransmission(true);
}

void sendHourToClockSlave() {
  Serial.printf("Sending hour to clock slave: %i \n", rtc.getHour());
  Wire.beginTransmission(4);

  uint8_t Buffer[2];
  Buffer[0] = 104;  // 104 is char -> h
  Buffer[1] = rtc.getHour();
  Wire.write(Buffer, 2);

  Wire.endTransmission();
}

void loop() {
  // Write message to the slave

  delay(1000);
  sendMinToClockSlave();
  sendHourToClockSlave();
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
