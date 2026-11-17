# mist_cooler_code

hardware in use : ESP32

this code the design for adaptive mist cooler project that turn on/off 
relay module according to dew point of the temperature sensor.

before compiling the code please change the setting in the module.h 
plese note that when select false in sensor setting the machine is allow
to work not matter what the sensor reading is and the code doesn't 
close the pump when it is up for a long time yet.

the code work by connect the relay which connect to the pump that pump the water to the mist
spreader. Motion sensor and temperature is option but if select true you should connect the sensor to the MCU.

please note that the pump must connect to NC of relay

if you are going to use wifi please store your data in wifikey.h

 todo :
 add feature to prevent on-off of switch rapidly when the value is near the threashold
 use millis?
 use water level sensor
 display status on display/oled
 
 inexperiment :
  record the temperature and humanity and sent to the google sheet
  define trigger range Dew point vs temp 

 done :
