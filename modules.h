#ifndef MODULES_H
#define MODULES_H
//set up the code and pins
inline bool motion_sensor = false;
inline bool temperature_sensor = true;
inline bool using_dewpoint = true;
inline bool enable_pump_safety = true;
inline bool using_sht_heater = false;
inline bool collect_data_in_sheet = false;
const int delay_ms = 1000;
const int PumpOnTime_min = 15;
const int PumpRestTime_min = 5;
const float sheet_sending_interval_s = 10;
const float set_temp = 24.0;
const float set_dew = 16.0;
// ending setup part


#define motion_pin 26
#define pump_relay 27
#define SERIAL_BAUD_RATE 115200
extern float temperature;
extern float humidity;
extern float dew_point;

//sht31 lib
#include "HardwareSerial.h"
#include "esp32-hal-gpio.h"
#include <math.h>
#include <Arduino.h>
#include <Wire.h>
#include <cmath>
#include "Adafruit_SHT31.h"
#include "sdkconfig.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "wifikey.h"

#include <cstdlib>
extern float temperature;
extern float humidity;
extern float dew_point;
extern bool enableHeater;

#define SENSOR_READ_INTERVAL_MS   30000UL   // how often to read + upload (ms)
#define WIFI_CONNECT_TIMEOUT_MS   15000UL   // max time to wait for WiFi to connect
#define HTTP_TIMEOUT_MS           10000UL   // max time to wait for the HTTP request
//normal
void pin_initialize();
float calculate_dew_point(float temp, float humi);
void collect_data_to_sheet();
//sht31
void sht31_setup();
void sht31_reading_loop();
//pir motion
int pir_status();
//pump
void pump_state_defier();
void pump_work();
void pump_safety();
//internet
void connectToWiFi();
bool isWiFiConnected();
bool sendToGoogleSheets(float temperature, float humidity, float dew);

#endif