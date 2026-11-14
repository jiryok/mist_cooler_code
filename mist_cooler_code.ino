#include"modules.h"


void setup() {
  pin_initialize();
  sht31_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  sht31_reading_loop();
  pump_state_defier();
  pump_safety();
  pump_work();
  delay(delay_ms);
}
