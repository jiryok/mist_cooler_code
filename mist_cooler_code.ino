#include"modules.h"


void setup() {
  pin_initialize();
  sht31_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  sht31_reading_loop();
  dew_point = calculate_dew_point(float temperature, humidity);
  pump_state_defier();
  pump_work();
  delay(delay_ms);
}
