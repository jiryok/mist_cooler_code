#include "modules.h"

void setup() {
  pin_initialize();
  if (collect_data_in_sheet) { // Fixed: Only connect at boot if sheet logging is enabled
    connectToWiFi();
  }
  sht31_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  sht31_reading_loop();
  sendToGoogleSheets(temperature, humidity, dew_point);
  pump_state_defier();
  pump_safety();
  pump_work();
  delay(delay_ms);
}
