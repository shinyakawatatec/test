/*************************************************************************
* File Name          : TOF sensor with ambient
* Author             : nakata
* Version            : V1.0
* Date               : 2024/07/01
* board              : M5Stack Arduino -> M5StampPico
* device             : M5StampPico - VL53L1CX
*************************************************************************/

#include <WiFi.h>
#include "Ambient.h"
#include <Wire.h>
#include <VL53L1X.h>

#define sensorVdd 25  // センサの電源ピン
#define TOFPIN 26     // TOFセンサのデータピン（必要に応じて変更）

VL53L1X tofSensor;

WiFiClient client;
Ambient ambient;

const char* ssid = "Buffalo-2G-1CB0";
const char* password = "rmpb8ege5ysr3";
//const char* ssid = "Buffalo-G-4E80";
//const char* password = "au6s5w4hrryeu";
unsigned int channelId = 78904;
const char* writeKey = "3fda767ab40ea697";

unsigned long interval = 3600; // unit: sec
//unsigned long interval = 5; // unit: sec test

void setup() {
  Serial.begin(115200);
  delay(1000);

  // WiFi Connect
  Serial.printf("Connecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected\n");

  // センサの電源をオン
  pinMode(sensorVdd, OUTPUT);
  digitalWrite(sensorVdd, HIGH);
  delay(2000); // センサが安定するまで待機

  Wire.begin();
  tofSensor.setTimeout(500);
  if (!tofSensor.init()) {
    Serial.println("Failed to detect and initialize sensor!");
    return;
  }

  tofSensor.setDistanceMode(VL53L1X::Long);
  tofSensor.setMeasurementTimingBudget(50000);
  tofSensor.startContinuous(50);

  int distance = tofSensor.read();
  if (distance == -1) {
    Serial.println("Failed to read from VL53L1X sensor!");
    return;
  }

  Serial.printf("Distance: %d mm\n", distance);

  ambient.begin(channelId, writeKey, &client);
  ambient.set(2, distance);
  ambient.send();

  // WiFi Disconnect
  WiFi.disconnect(true);
  Serial.printf("WiFi disconnected\n");

  // センサの電源をオフ
  digitalWrite(sensorVdd, LOW);

  // Deep Sleep
  esp_sleep_enable_timer_wakeup(interval * 1000 * 1000);
  esp_deep_sleep_start();
}

void loop() {
}
