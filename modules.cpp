#include "modules.h"

#if CONFIG_ESP_WIFI_REMOTE_ENABLED
#error "WPA2-Enterprise requires a board with native Wi-Fi support"
#endif

float temperature;
float humidity;
float dew_point;
bool enableHeater = false;
uint8_t loopCnt = 0;
bool pump_on = false;
bool pump_rest = false;

unsigned long pump_run_time_ms = 0;
long pump_rest_time_ms = PumpRestTime_min * 60UL * 1000UL;
unsigned long pump_max_run_time_ms = PumpOnTime_min * 60UL * 1000UL;  
float cmt_sending_time = 0;

Adafruit_SHT31 sht31 = Adafruit_SHT31();

void pin_initialize() {
  Serial.begin(SERIAL_BAUD_RATE);
  //while (!Serial)
  //  delay(10);     // will pause Zero, Leonardo, etc until serial console opens
  pinMode(motion_pin, INPUT);
  digitalWrite(pump_relay, LOW);
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
  if (using_sht_heater)
  {
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
  dew_point = calculate_dew_point(temperature, humidity);
  Serial.printf("dew point : ");
  Serial.println(dew_point);
}

void pump_state_defier() {
  bool motion_allow = 0;
  bool temperature_allow = 0;

  // Fixed: Safety guard if SHT31 fails to read
  if (temperature_sensor && isnan(temperature)) {
    pump_on = 0;
    return;
  }
  if (using_dewpoint && isnan(dew_point)) {
    pump_on = 0;
    return;
  }

  if ((temperature_sensor && (temperature > standby_temp)) && allow_standby_temp)
  {
    pump_on = 1;
    return;
  }
  if (motion_sensor){
    int a = pir_status();
    motion_allow = a;
  } else {
    motion_allow = 1;
  }

  if (using_dewpoint)
  {
    if (((!using_dewpoint) || (dew_point < set_dew)) && (temperature > set_temp))
    {
      temperature_allow = 1;
    }
    else
    {
      temperature_allow = 0;
    }
  }
  else if (temperature_sensor)
  {
    if (temperature > set_temp)
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

void pump_safety()
{
  if (enable_pump_safety)
  {
    if (pump_rest)
    {
      pump_rest_time_ms -= delay_ms;
      pump_on = false;
      if (pump_rest_time_ms <= 0)
      {
        pump_rest = false;
        pump_run_time_ms = 0;
        pump_rest_time_ms = PumpRestTime_min * 60UL * 1000UL;
      }
      return;
    }
    else if (pump_on)
    {
      // Reset idle rest countdown whenever the pump is actively running
      pump_rest_time_ms = PumpRestTime_min * 60UL * 1000UL;
      pump_run_time_ms += delay_ms;
      if (pump_run_time_ms >= pump_max_run_time_ms)
      {
        pump_rest = true;
        pump_on = false; // Fixed: Immediately turn off pump on the cycle rest begins
        pump_run_time_ms = 0;
      }
      return;
    }
    else
    {
      // Pump is naturally idle: count idle time as resting time!
      if (pump_run_time_ms > 0)
      {
        pump_rest_time_ms -= delay_ms;
        if (pump_rest_time_ms <= 0)
        {
          // Rested naturally for the full PumpRestTime_min: clear accumulated run time
          pump_run_time_ms = 0;
          pump_rest_time_ms = PumpRestTime_min * 60UL * 1000UL;
        }
      }
      else
      {
        // Prevent negative underflow when pump_run_time_ms is already 0
        pump_rest_time_ms = PumpRestTime_min * 60UL * 1000UL;
      }
    }
  }
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
  if (isnan(temp) || isnan(humi) || humi <= 0 || humi > 100)
  {
    return NAN;
  }
  float gamma = log(humi/100.0) + (17.62*temp)/(243.12+temp);
  return (243.12*gamma)/(17.62-gamma);
}

int pir_status() {
  return digitalRead(motion_pin);
}

//#wifi
void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(wifi_name);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  if (using_enterprise_wifi)
  {
    WiFi.begin(wifi_name, WPA2_AUTH_PEAP, eap_iden, wifi_user_name, wifi_password);
  }
  else {
    WiFi.begin(wifi_name, wifi_password);
  }
  

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED &&
         millis() - startAttempt < WIFI_CONNECT_TIMEOUT_MS) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected. IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection failed (will retry on next cycle).");
  }
}

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

unsigned long last_sheet_send_time = 0;

bool sendToGoogleSheets(float temperature, float humidity, float dew) {
  if (!collect_data_in_sheet) {
    return false;
  }

  // Fixed: Do not upload invalid NAN readings
  if (isnan(temperature) || isnan(humidity)) {
    return false;
  }

  // ตรวจสอบรอบเวลาส่งข้อมูล (แปลง sheet_sending_interval_s เป็น ms)
  if (millis() - last_sheet_send_time < (unsigned long)(sheet_sending_interval_s * 1000.0f)) {
    return false;
  }

  if (!isWiFiConnected()) {
    connectToWiFi();
    if (!isWiFiConnected()) {
      Serial.println("WiFi not connected — skipping upload.");
      return false;
    }
  }

  WiFiClientSecure client;
  client.setInsecure(); // ข้ามการตรวจ certificate

  HTTPClient http;
  http.setTimeout(HTTP_TIMEOUT_MS);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // ✅ บังคับตาม HTTP 302 Redirect ของ Google

  String url = String(sheet_link) +
               "?temperature=" + String(temperature, 2) +
               "&humidity=" + String(humidity, 2) +
               "&dew_point=" + String(dew, 2);

  bool success = false;

  if (http.begin(client, url)) {
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("HTTP response code: %d\n", httpCode);
      Serial.println("Response: " + http.getString());
      // สำหรับ Google Script เมื่อ redirect สำเร็จจะได้รับ 200 OK
      success = (httpCode == HTTP_CODE_OK);
      if (success) {
        last_sheet_send_time = millis(); // ✅ รีเซ็ตเวลานับถอยหลังรอบใหม่เมื่อส่งสำเร็จ
      }
    } else {
      Serial.print("HTTP request failed: ");
      Serial.println(http.errorToString(httpCode));
    }

    http.end();
  } else {
    Serial.println("Unable to connect to Google Script URL.");
  }
 
  return success;
}
