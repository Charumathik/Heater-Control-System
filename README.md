Embedded Heater Control System
An ESP32-based heater automation project that uses a DS18B20 temperature sensor and a relay to maintain a target temperature. The system operates via a state machine (Idle, Heating, Stabilizing, Target Reached, Overheat) with overheat protection and visual warning via a blinking LED.

🚀 Features
- Accurate Temperature Monitoring — DS18B20 digital sensor (±0.5°C)
- Automatic Heater Control — via relay module
- Safety Override — overheat cut‑off at ≥75 °C
- Visual Alert — blinking LED in overheat condition
- Clean State Machine Logic — for predictable, stable operation
- UART Serial Logging — for debugging and monitoring
- Optional BLE Module — for mobile monitoring/control (code included but commented)

🛠 Hardware Requirements
1. ESP32 Dev Board
2. DS18B20 Temperature Sensor (+4.7 kΩ pull‑up resistor)
3. Relay Module (to control heater)
4. Red LED (+220 Ω resistor)
5. Breadboard, jumper wires, power supply

🔌 Pin Mapping
Component----------Pin GPIO
DS18B20-DQ	       GPIO 4
Relay--IN          GPIO 26
LED +    	         GPIO 2
GND/3.3V	         Common

📊 System States
. IDLE – Heater OFF, waiting for temperature drop
. HEATING – Heater ON until near target temp
. STABILIZING – Maintaining temperature within tolerance
. TARGET REACHED – Heater OFF, stable temp maintained
. OVERHEAT – Heater OFF + blinking LED warning

🛠 How It Works
1. DS18B20 continuously reads temperature.
2. ESP32 compares readings with target and tolerance values.
3. Relay turns the heater ON/OFF depending on the state.
4. Safety cutoff triggers at ≥75 °C, LED blinks every 500 ms.

📂 Repository Contents
sketch.ino — Main firmware code for ESP32
diagram.json — Wokwi circuit simulation wiring file
HEATER-CONTROL-SYSTEM.pdf — Full design documentation
README.md — This file

🔗 Project Links
Wokwi Simulation: https://wokwi.com/projects/439089545074845697

▶️ How to Run
Open the .ino file in Arduino IDE.
Select the ESP32 Dev Module as the board.
Connect your ESP32 board via USB.
Install required libraries (OneWire and DallasTemperature).
Upload the sketch to the board.
Monitor the output via the Serial Monitor at 115200 baud.


