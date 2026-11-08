/*
wating


*/
#include"modules.h"
float dew_point;
float temperature;
float humidity;
bool enableHeater = false;
uint8_t loopCnt = 0;
bool pump_on;



Adafruit_SHT31 sht31 = Adafruit_SHT31();

void pin_initialize() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);     // will pause Zero, Leonardo, etc until serial console opens
  pinMode(motion_pin, INPUT);
  pinMode(pump_relay, OUTPUT);
}


void sht31_setup() {

  Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");
}


void sht31_reading_loop() {
  temperature = sht31.readTemperature();
  humidity = sht31.readHumidity();
  Serial.println("SHT31 result");

  if (! isnan(temperature)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.print(temperature); Serial.print("\t\t");
  } else { 
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(humidity)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(humidity);
  } else { 
    Serial.println("Failed to read humidity");
  }


  // Toggle heater enabled state every 30 seconds
  // An ~3.0 degC temperature increase can be noted when heater is enabled
  if (loopCnt >= 30) {
    enableHeater = !enableHeater;
    sht31.heater(enableHeater);
    Serial.print("Heater Enabled State: ");
    if (sht31.isHeaterEnabled())
      Serial.println("ENABLED");
    else
      Serial.println("DISABLED");

    loopCnt = 0;
  }
  loopCnt++;
}

void pump_state_defier() {
  bool motion_allow = 0;
  bool temperature_allow = 0;

  if (motion_sensor){
    int a = pir_status();
    motion_allow = a;
  } else {
    motion_allow = 1;
  }

  if (temperature_sensor){
    if (!((dew_point < 16) && (dew_point > 7))) 
    {
      temperature_allow = 1;
    } 
    else 
    {
      temperature_allow = 0;
    }
  } 
  else 
  {
    temperature_allow = 1;
  }
  pump_on = temperature_allow && motion_allow;
}

void pump_work() 
{
  if (pump_on)
  {
    digitalWrite(pump_relay, HIGH);
    Serial.println("PUMP ON");
  }
  else 
  {
    digitalWrite(pump_relay, LOW);
    Serial.println("PUMP OFF");
  }
}

float calculate_dew_point(float temp, float humi)
{
  if (humidity <= 0 || humidity > 100) 
  {
   return NAN;
  }
  float gamma = log(humi/100.0) + (17.62*temp)/(243.12+temp);
  return (243.12*gamma)/(17.62-gamma);
}

int pir_status() {
  return digitalRead(motion_pin);
}

