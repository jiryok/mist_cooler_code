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

# 🌫️ ESP32 Smart Mist Cooler System

An automated, IoT-enabled mist cooling controller built on the **ESP32** platform. This system monitors ambient temperature and relative humidity using an **SHT31 sensor**, calculates the real-time **dew point** to prevent over-humidification, detects human presence via a **PIR motion sensor**, and logs environmental data directly to **Google Sheets** over **WPA2-Enterprise (PEAP) Wi-Fi**.

---

## ✨ Key Features

- **Precision Environmental Sensing:** Reads temperature (°C) and relative humidity (%) via I2C using the **Adafruit SHT31** sensor, with optional built-in sensor heater toggling.
- **Dew Point Calculation:** Uses the Magnus-Tetens approximation to prevent misting when the air is too saturated, avoiding condensation and wet surfaces.
- **Smart Pump Control:** Triggers the misting pump relay based on configurable temperature thresholds, dew point limits, and optional PIR motion detection.
- **Pump Duty-Cycle Safety:** Protects the misting pump from overheating by enforcing a maximum continuous run time (`PumpOnTime_min`) followed by a mandatory cool-down rest period (`PumpRestTime_min`).
- **Enterprise Wi-Fi Support:** Connects directly to organizational/university networks using **WPA2-Enterprise (PEAP)** authentication (Identity, Username, Password).
- **Cloud Data Logging:** Periodically uploads temperature and humidity readings to **Google Sheets** via a Google Apps Script Web App webhook over HTTPS.

---

## 🛠️ Hardware Requirements

| Component | Description / Model |
|---|---|
| **Microcontroller** | ESP32 Development Board (with native Wi-Fi support) |
| **Temp & Humidity Sensor** | Sensirion SHT31-D Breakout (I2C) |
| **Motion Sensor** | HC-SR501 PIR Motion Sensor (Optional) |
| **Relay Module** | 1-Channel 5V/3.3V Opto-isolated Relay Module |
| **Actuator** | High-pressure Mist Pump / Solenoid Valve |

---

## 🔌 Pinout & Wiring

| ESP32 Pin | Component Pin | Notes |
|---|---|---|
| **GPIO 21 (SDA)** | SHT31 `SDA` | Default I2C Data (`0x44` address) |
| **GPIO 22 (SCL)** | SHT31 `SCL` | Default I2C Clock |
| **GPIO 26** | PIR Sensor `OUT` | Configured as `INPUT` (`motion_pin`) |
| **GPIO 27** | Relay `IN` | Configured as `OUTPUT` (`pump_relay`, Active HIGH) |
| **3.3V / 5V** | `VCC` | 3.3V for SHT31; 5V for Relay & PIR |
| **GND** | `GND` | Common Ground |

---

## 📁 Project Structure

```text
mist_cooler_code/
├── mist_cooler_code.ino   # Main Arduino sketch (setup & main loop)
├── modules.h              # Pin definitions, constants, flags, and declarations
├── modules.cpp            # Core logic for SHT31, Dew Point, Pump Safety, Wi-Fi & Sheets
└── README.md              # Project documentation
