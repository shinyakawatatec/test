/*************************************************************************
* File Name          : soil moisture and thermocouple sensors with ambient
* Author             : nakata
* Version            : V1.2
* Date               : 2024/07/18
* board              : M5Stack Arduino -> M5StampPico
* device             : M5StampPico - DiyStudio, K type thermocouple
*************************************************************************/

#include <WiFi.h>
#include "Ambient.h"

#define sensorVdd 25

WiFiClient client;
Ambient ambient;

const char* ssid = "Buffalo-2G-1CB0"; //第４工場
const char* password = "rmpb8ege5ysr3";
//const char* ssid = "Buffalo-G-4E80"; //事務所UTM上
//const char* password = "au6s5w4hrryeu";


unsigned int channelId = 78904;
const char* writeKey = "3fda767ab40ea697";

/*
//動作確認用
unsigned int channelId = 81004;
const char* writeKey = "c9b3cff4e14ad866";
*/

unsigned long interval = 3600; // unit: sec
//unsigned long interval = 1; // unit: sec    //動作確認用短サイクル

float V_ref = 3.3; // Initial reference voltage for ADC

void setup() {
  Serial.begin(115200);
  delay(1000);

  // WiFi Connect
  Serial.printf("Connecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected\n");

  pinMode(sensorVdd, OUTPUT);
  digitalWrite(sensorVdd, HIGH);
  delay(500);

// delay(5000);  //動作確認用
  
  // Read battery voltage
  int a2 = analogRead(32); // Battery voltage (changed to pin 32)
  float voltage_a2 = ((a2 / 4095.0) * 3.3 + 0.14) * 2; // Assuming voltage divider for battery measurement
  V_ref = voltage_a2;

  // Read other analog values
  int a0 = analogRead(36); // Soil moisture sensor (pin 36)
  int a1 = analogRead(33); // K type thermocouple (changed to pin 33)

  // Convert to voltage and temperature
  float voltage_a1 = ((a1 / 4095.0) * 3.3)+0.14;
  float temperature_a1 = ((voltage_a1 - 1.25) / 0.005) + 4;

  Serial.printf("Soil moisture sensor value: %d\n", a0);
  Serial.printf("K type thermocouple value: %f℃\n", temperature_a1);
  Serial.printf("Battery voltage: %fV\n", voltage_a2);


  // Send data to Ambient
  ambient.begin(channelId, writeKey, &client);
  ambient.set(1, a0);
  ambient.set(4, temperature_a1);
  ambient.set(5, voltage_a2);
  ambient.send();

  // WiFi Disconnect
  WiFi.disconnect(true);
  Serial.printf("WiFi disconnected\n");

  // Deep Sleep
  esp_sleep_enable_timer_wakeup(interval * 1000 * 1000);
  esp_deep_sleep_start();
}

void loop() {
}
