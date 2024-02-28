// #include <ESP32Time.h>
// #include <Stepper.h>
// #include <WiFi.h>
// #include <Wire.h>

// #include "time.h"

// byte TxByte = 0xAA;

// const char* ssid = "EscapeFactoryWireless";
// const char* password = "T1m3T03$c@p3";

// const char* ntpServer = "pool.ntp.org";
// const long gmtOffset_sec = 0;
// const int daylightOffset_sec = 3600;

// ESP32Time rtc(daylightOffset_sec);
// struct tm timeinfo;

// int previousDaySync = 0;

// int currentHour = 12;
// int currentMinute = 12;

// void syncTime() {
//   Serial.println("SYNCING TIME...");

//   // Connect to Wi-Fi
//   Serial.print("Connecting to ");
//   Serial.println(ssid);
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.println("WiFi connected.");

//   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

//   if (getLocalTime(&timeinfo)) {
//     rtc.setTimeStruct(timeinfo);
//   }

//   previousDaySync = rtc.getDay();

//   // disconnect WiFi as it's no longer needed
//   WiFi.disconnect(true);
//   WiFi.mode(WIFI_OFF);
// }

// void I2C_TxHandler(void) { Wire.write(TxByte); }
// const int stepsPerRevolution = 720;

// // initialize the stepper library on pins 8 through 11:
// Stepper myStepper(stepsPerRevolution, 16, 17, 18, 19);

// void setup() {
//   Serial.begin(9600);
//   Serial.println("STARTING...");
//   // syncTime();
//   // Wire.begin(0x55);
//   // Wire.onRequest(I2C_TxHandler);
//   // STEPPER MOTOR
//   myStepper.setSpeed(40);
// }

// void loop() {
//   myStepper.step(stepsPerRevolution);
//   // delay(500);

//   // // step one revolution in the other direction:
//   // Serial.println("counterclockwise");
//   // myStepper.step(-stepsPerRevolution);
//   // delay(500);

//   // if (rtc.getDay() != previousDaySync) {
//   //   // if a day has passed sync the time with the servers again
//   //   syncTime();
//   // }
//   // if (currentHour != rtc.getHour(false)) {
//   //   // if the previous hour doesn't equal the current hour on display
//   anymore
//   // }

//   // if (currentMinute != rtc.getMinute()) {
//   //   // if the previous minute doesn't equal the current minute on display
//   //   // anymore
//   // }

//   // currentHour = rtc.getHour(false);
//   // currentMinute = rtc.getMinute();

//   // delay(1000);
// }
// #include <Wire.h>

// void setup() {
//   Wire.begin();  // join i2c bus (address optional for master)
// }

// void loop() {
//   Wire.beginTransmission(3 );  // transmit to device #4
//   Wire.write("x is ");        // sends five bytes
//   Wire.endTransmission();     // stop transmitting
//   delay(500);
// }