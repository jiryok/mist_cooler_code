#include "modules.h"

void setup() {
  pin_initialize();
  if (collect_data_in_sheet) { // Fixed: Only connect at boot if sheet logging is enabled
    connectToWiFi();
  }
  sht31_setup();
}

void loop() {
  sht31_reading_loop();
  sendToGoogleSheets(temperature, humidity, dew_point);
  pump_state_defier();
  pump_safety();
  pump_work();
  
  // เรียกใช้ฟังก์ชัน Serial Print รวมที่สร้างขึ้นใหม่
  print_system_status();
  
  delay(delay_ms);
}
