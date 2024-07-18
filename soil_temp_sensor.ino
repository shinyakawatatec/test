/*************************************************************************
* File Name          : soil moisture and thermocouple sensors with ambient
* Author             : nakata
* Version            : V1.0
* Date               : 2024/07/05
* board              : M5Stack Arduino -> M5StampPico
* device             : M5StampPico - DiyStudio, K type thermocouple
*************************************************************************/

#include <WiFi.h>
#include "Ambient.h"
#include <SPI.h>
#include "MCP3004.h"

#define sensorVdd 25

#define MCP3004_SS 19
MCP3004 mcp3004(MCP3004_SS);

WiFiClient client;
Ambient ambient;

const char* ssid = "Buffalo-2G-1CB0"; //第４工場
const char* password = "rmpb8ege5ysr3";
//const char* ssid = "Buffalo-G-4E80"; //事務所UTM上
//const char* password = "au6s5w4hrryeu";


unsigned int channelId = 78904;
const char* writeKey = "3fda767ab40ea697";

unsigned long interval = 3600; // unit: sec
//unsigned long interval = 5; // unit: sec    //動作確認用短サイクル

const float V_ref = 3.03; // Reference voltage for ADC

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
//  delay(50000); //to measure the Vref
  

  SPI.begin(18, 36, 26, 19); // SPI.begin(sck, miso, mosi, ss);
  mcp3004.begin();

  int a0 = mcp3004.read(0);
  int a1 = mcp3004.read(1);

  float voltage_a1 = (a1 / 1023.0) * V_ref;
  float temperature_a1 = (voltage_a1 - 1.25) / 0.005;
 
  Serial.printf("Channel 0 value: %d\n", a0);
  Serial.printf("Channel 1 value: %f℃\n", temperature_a1);

  ambient.begin(channelId, writeKey, &client);
  ambient.set(1, a0);
  ambient.set(4, temperature_a1);
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
