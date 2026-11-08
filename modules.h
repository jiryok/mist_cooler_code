#ifndef MODULES_H
#define MODULES_H
//set up the code and pins
inline bool motion_sensor = false;
inline bool temperature_sensor = true;
#define delay_ms 1000

#define motion_pin 26
#define pump_relay 27


//sht31 lib
#include "HardwareSerial.h"
#include "esp32-hal-gpio.h"
#include <math.h>
#include <Arduino.h>
#include <Wire.h>
#include <cmath>
#include "Adafruit_SHT31.h"
extern float temperature;
extern float humidity;
extern float dew_point;
extern bool enableHeater;
//normal
void pin_initialize();
float calculate_dew_point(float temp, float humi);
//sht31
void sht31_setup();
void sht31_reading_loop();
//pir motion
int pir_status();
//pump
void pump_state_defier();
void pump_work();

#endif