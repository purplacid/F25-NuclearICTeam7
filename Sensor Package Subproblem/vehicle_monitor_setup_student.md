# 🚚 Nuclear Transport Vehicle State Monitoring Challenge

**Welcome!**

Congratulations! You've been chosen to join an elite engineering team at Canadian Nuclear Labs (CNL). Your mission? Create an innovative system for real-time monitoring of vehicle and container conditions during nuclear waste transport across Canada.

Transport trucks will face tough conditions—vibrations from uneven roads, shifting weight distribution during loading, harsh weather, and potential shielding failures. Your solution will help protect public safety and environmental health by promptly identifying any risks.

---

## 📖 Challenge Overview

Design a comprehensive sensor package using Arduino Uno R4 Minima, Grove Sensor Kit, and additional sensors to monitor relevant metrics for transporting nuclear material.

Examples of relevant emergencies might include:
- Crashes
- Internal temperature and humidity
- Radiation levels (simulated using 433MHz RF EMR)

Alert drivers and authorities immediately if anomalies occur!

---

## ⚙️ Documentation and Reference Material

- **Microcontroller:** Arduino Uno R4 Minima
- **Grove Sensor Kit:** [ECE198 Grove Sensor Setup Guide](https://github.com/IdeasClinicUWaterloo/Technologies-Utilized-for-Idea-s-Clinic-Challenges/blob/main/Seed%20Grove%20Kit/GUIDE.md)
- **Additional Sensors:**

    - [DFRobot BMX160 9-Axis IMU](https://wiki.dfrobot.com/BMX160_9-axis_Sensor_Module_SKU_SEN0373)
    - [Adafruit AHT20 Temperature and Humidity Sensor](https://learn.adafruit.com/adafruit-aht20)
    - [Hx53/HXM121 Infrared Transmitter/Receiver](https://mschoeffler.com/2021/05/01/arduino-tutorial-ir-transmitter-and-ir-ir-receiver-hx-m121-hx-53-ky-005-ky-022-keyes-iduino-open-smart/)
- ** BreadBoard Basics ** [https://www.build-electronic-circuits.com/breadboard/]

      *Detailed setup and code instructions for these modules are included in the sections below.*

---

## 🚀 Getting Started

Follow these steps to set up your vehicle state monitoring solution:

### 1. 🛠 Arduino and Grove Shield Setup

- Connect your Arduino Uno R4 Minima to your computer via USB.
- Attach the Grove Shield onto your Arduino for simplified sensor connections.
- Ensure your Arduino IDE is installed: [Arduino IDE Download](https://www.arduino.cc/en/software).

### 2. 📡 Sensor Connections

For Grove kit sensor connections - refer to the provided `https://github.com/IdeasClinicUWaterloo/Technologies-Utilized-for-Idea-s-Clinic-Challenges/blob/main/Seed%20Grove%20Kit/GUIDE.md`.
Follow these specific instructions for each additional sensor:

#### DFRobot BMX160 9-Axis IMU

-   **Connection:**
-       VCC -> 5V
-       GND -> GND
-       SLA -> SLA
-       SDA -> SDA  
-   **Library:** Install `DFRobot_BMX160` via the Arduino IDE Library Manager.
-   **Setup Notes:** This sensor provides accelerometer, gyroscope, and magnetometer data. It communicates via I2C. Ensure no other I2C devices on the same port have address conflicts (though unlikely for these modules).
-   **Reference:** See [DFRobot Guide](https://wiki.dfrobot.com/BMX160_9-axis_Sensor_Module_SKU_SEN0373) for basic usage examples.

#### Adafruit AHT20 Temperature and Humidity Sensor

-   **Connection:**
-       VCC -> 5V
-       GND -> GND
-       SLA -> SLA
-       SDA -> SDA 
-   **Setup Notes:** Provides accurate ambient temperature and humidity. Communicates via I2C.
-   **Reference:** See [Adafruit AHT20 Guide](https://learn.adafruit.com/adafruit-aht20) for library usage.

#### Hx53/HXM121 Infrared Transmitter/Receiver

-   **Connection:** Digital pins on Grove Shield.
    * **Transmitter (Hx53):** Connect VCC to 5V, GND to GND, and SIG to an Arduino Digital Pin (e.g., D6).
    * **Receiver (HXM121):** Connect VCC to 5V, GND to GND, and SIG to another Arduino Digital Pin (e.g., D7).
-   **Library:** No dedicated library is strictly required; simple `digitalRead()` and `digitalWrite()` are used.
-   **Setup Notes:** These modules typically send/receive simple ON/OFF pulses. The transmitter sends an IR signal when its SIG pin is HIGH, and the receiver's SIG pin goes LOW when it detects an IR signal. Useful for line-of-sight detection or short-range communication.
-   **Reference:** See [IR Transmitter/Receiver Guide](https://www.elecfreaks.com/wiki/index.php?title=IR_Transmitter_and_Receiver_Module) for wiring and basic code examples.

## Testing Stations

 - Nuclear waste will be simulated using an IR Emitter
 - The radioactive material will be placed inside a 3x3x3 nuclear waste containor with a small hole to simulate a radiation leak
 - Various films will be prepared to block radiation and simulate varying levels of radation intensity
 - The nuclear wate package and your hardware solution will be fitted onto an RC car where it will undergo various scenarios including a crash, heatwave, tipping over and speedbumps


    lcd.setRGB(255, 0, 0);
    lcd.setCursor(0,0);
    lcd.print("!!! ALERT !!!");
    lcd.setCursor(0,1);
    lcd.print("Anomaly Detected!");
    // You might also add a buzzer or external LED here
}
