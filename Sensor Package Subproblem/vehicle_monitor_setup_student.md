# 🚚 Nuclear Transport Vehicle State Monitoring Challenge

**Welcome!**

Congratulations! You've been chosen to join an elite engineering team at Canadian Nuclear Labs (CNL). Your mission? Create an innovative system for real-time monitoring of vehicle and container conditions during nuclear waste transport across Canada.

Transport trucks will face tough conditions—vibrations from uneven roads, shifting weight distribution during loading, harsh weather, and potential shielding failures. Your solution will help protect public safety and environmental health by promptly identifying any risks.

---

## 📖 Challenge Overview

Design a comprehensive sensor package using Arduino Uno R4 Minima, Grove Sensor Kit, and additional sensors to monitor relevant metrics for transporting nuclear material.

Examples of relevant metrics include:
- Vibrations and orientation
- Internal temperature and humidity
- Radiation levels (simulated using 433MHz RF EMR)

Alert drivers and authorities immediately if anomalies occur!

---

## ⚙️ Documentation and Reference Material

- **Microcontroller:** Arduino Uno R4 Minima
- **Grove Sensor Kit:** [ECE198 Grove Sensor Setup Guide](ECE198SensorGuide.pdf)
- **Additional Sensors:**
    - [DFRobot BMX160 9-Axis IMU](https://wiki.dfrobot.com/BMX160_9-axis_Sensor_Module_SKU_SEN0373)
    - [Adafruit AHT20 Temperature and Humidity Sensor](https://learn.adafruit.com/adafruit-aht20)
    - [Hx53/HXM121 Infrared Transmitter/Receiver](https://mschoeffler.com/2021/05/01/arduino-tutorial-ir-transmitter-and-ir-ir-receiver-hx-m121-hx-53-ky-005-ky-022-keyes-iduino-open-smart/)
    - **TX118S-4/RX480-E-WQ 433MHz RF Transmitter/Receiver:** [Product Page (Amazon.ca)](https://www.amazon.ca/QIACHIP-Wireless-Receiver-Transmitter-Learning/dp/B06ZZ1Z6R7?sr=8-10)
      *Detailed setup and code instructions for these modules are included in the sections below.*

- **RF Shielding:** Aluminum foil can be used to attenuate RF signals for testing purposes.

---

## 🚀 Getting Started

Follow these steps to set up your vehicle state monitoring solution:

### 1. 🛠 Arduino and Grove Shield Setup

- Connect your Arduino Uno R4 Minima to your computer via USB.
- Attach the Grove Shield onto your Arduino for simplified sensor connections.
- Ensure your Arduino IDE is installed: [Arduino IDE Download](https://www.arduino.cc/en/software).

### 2. 📡 Sensor Connections

For Grove kit sensor connections - refer to the provided `ECE198 Grove Sensor Setup Guide.pdf`.
Follow these specific instructions for each additional sensor:

#### DFRobot BMX160 9-Axis IMU

-   **Connection:** I2C port on Grove Shield (e.g., I2C-1, I2C-2)
-   **Library:** Install `DFRobot_BMX160` via the Arduino IDE Library Manager.
-   **Setup Notes:** This sensor provides accelerometer, gyroscope, and magnetometer data. It communicates via I2C. Ensure no other I2C devices on the same port have address conflicts (though unlikely for these modules).
-   **Reference:** See [DFRobot Guide](https://wiki.dfrobot.com/BMX160_9-axis_Sensor_Module_SKU_SEN0373) for basic usage examples.

#### Adafruit AHT20 Temperature and Humidity Sensor

-   **Connection:** I2C port on Grove Shield (e.g., I2C-1, I2C-2, sharing with BMX160 is fine as they have different addresses).
-   **Library:** Install `Adafruit_AHTX0` via the Arduino IDE Library Manager.
-   **Setup Notes:** Provides accurate ambient temperature and humidity. Communicates via I2C.
-   **Reference:** See [Adafruit AHT20 Guide](https://learn.adafruit.com/adafruit-aht20) for library usage.

#### Hx53/HXM121 Infrared Transmitter/Receiver

-   **Connection:** Digital pins on Grove Shield.
    * **Transmitter (Hx53):** Connect VCC to 5V, GND to GND, and SIG to an Arduino Digital Pin (e.g., D6).
    * **Receiver (HXM121):** Connect VCC to 5V, GND to GND, and SIG to another Arduino Digital Pin (e.g., D7).
-   **Library:** No dedicated library is strictly required; simple `digitalRead()` and `digitalWrite()` are used.
-   **Setup Notes:** These modules typically send/receive simple ON/OFF pulses. The transmitter sends an IR signal when its SIG pin is HIGH, and the receiver's SIG pin goes LOW when it detects an IR signal. Useful for line-of-sight detection or short-range communication.
-   **Reference:** See [IR Transmitter/Receiver Guide](https://www.elecfreaks.com/wiki/index.php?title=IR_Transmitter_and_Receiver_Module) for wiring and basic code examples.

#### DFRobot GNSS Module (I2C)

-   **Connection:** I2C port on Grove Shield (e.g., I2C-1, I2C-2).
-   **Library:** `DFRobot_GNSS` (provided in the code, or can be found on DFRobot's website).
-   **Setup Notes:** This module provides GPS coordinates, altitude, speed, and time. Ensure a clear line of sight to the sky for optimal satellite acquisition. Initial fix might take some time.
-   **Reference:** Consult DFRobot documentation for specific `DFRobot_GNSS` library usage.

#### TX118S-4/RX480-E-WQ 433MHz RF Transmitter/Receiver (Simulated Radiation)

This pair of modules will simulate the detection of nuclear radiation. The transmitter will be the "radiation source" (wrapped in foil to limit its range), and the receiver on your main sensor package will be the "detector."

---

##### **Antenna Selection:**

Your package came with two types of coiled spring antennas: a "tighter coil" and a "looser coiled spring." For optimal 433MHz performance, you want an antenna that is electrically close to a quarter-wavelength (approx. 17.3 cm).

* **For the Transmitter (TX-118S):** Use pliers to carefully stretch the tightly coiled spring to make it longer. Next use pliers to modify the end of the coil so it fits into a breadboard, then connect it to the `ANT` pin on the TX module.
* **For the Receiver (RX480-E4):** use the **looser coiled spring antenna**. To maximize reception of the RF signal, modify the antenna to be closer to the ideal length. Better reception means more reliable detection of the (attenuated) transmitter signal. Solder it securely to the `ANT` pad on the module.

---

##### **A. Transmitter Setup (TX-118S) - On a separate Arduino board**

This Arduino will simulate the "nuclear radiation source" that your team needs to locate.

1.  **Hardware Connections:**
    * **Arduino Board:** Use a separate Arduino Uno, Nano, or similar board for this.
    * **TX-118S Module:**
        * `VCC (+V)`: Connect to Arduino **5V** pin.
        * `GND`: Connect to Arduino **GND** pin.
        * `K1`: Connect to Arduino Digital Pin **7** (or any available digital pin). When this pin is pulled LOW, the module transmits its encoded signal.
        * `ANT`: Solder the **looser coiled spring antenna** here.

    *Example Wiring:*
    ```
    TX-118S VCC --- Arduino 5V
    TX-118S GND --- Arduino GND
    TX-118S K1  --- Arduino Digital Pin 7
    TX-118S ANT --- Looser coiled antenna
    ```

2.  **Transmitter Code:**
    Upload this code to your separate Arduino board. It will continuously send "radiation" pulses.

    ```arduino
    // --- Transmitter Code (for a separate Arduino board) ---
    // This board acts as the hidden "nuclear radiation source."

    const int TRANSMIT_PIN = 7; // Connect TX-118S K1 to Arduino Digital Pin 7

    void setup() {
      pinMode(TRANSMIT_PIN, OUTPUT);
      digitalWrite(TRANSMIT_PIN, HIGH); // K1 is HIGH by default (no transmission)
      Serial.begin(9600);
      Serial.println("433MHz Transmitter Ready!");
      Serial.println("Continuously sending 'radiation' signal...");
    }

    void loop() {
      // To simulate a continuous signal, rapidly toggle the TRANSMIT_PIN.
      // The EV1527 encoder in the TX-118S will handle the encoding automatically.
      // We'll keep the pin LOW for a very short pulse to trigger a transmission,
      // then HIGH for a short period before the next pulse.
      digitalWrite(TRANSMIT_PIN, LOW);  // Pull K1 LOW to transmit
      delay(20);                       // Short pulse to ensure transmission (adjust if needed)
      digitalWrite(TRANSMIT_PIN, HIGH); // Pull K1 HIGH
      delay(100);                      // Delay before next transmission. Shorter delay = more pulses, perceived as stronger signal.
    }
    ```

3.  **Attenuation (Critical for Challenge):**
    After verifying the transmitter works, **wrap the TX-118S module (and its antenna) thoroughly in aluminum foil.** The more layers and the tighter you wrap it, the more the signal will be attenuated, making the "radiation source" harder to detect from a distance. Experiment with the amount of foil to achieve the desired detection range for your challenge.

---

##### **B. Receiver Setup (RX480-E4) - On your main Sensor Package Arduino**

This Arduino will integrate the "radiation detector" into your main sensor package.

1.  **Hardware Connections:**
    * **Arduino Board:** Your existing Sensor Package Arduino (Uno R4 Minima with Grove Shield).
    * **RX480-E4 Module:**
        * `+V`: Connect to Arduino **5V** pin on your Grove Shield.
        * `GND`: Connect to Arduino **GND** pin on your Grove Shield.
        * `VT (Output)`: Connect to Arduino Digital Pin **2**. This pin supports external interrupts on Arduino Uno R4 Minima, which is crucial for reliably counting received signals.
        * `D0, D1, D2, D3`: These pins are not strictly needed for this basic signal strength simulation, as the `VT` pin provides a combined signal detection. You can leave them unconnected.
        * `ANT`: Solder the **looser coiled spring antenna** here.

    *Example Wiring:*
    ```
    RX480-E4 +V --- Arduino 5V
    RX480-E4 GND --- Arduino GND
    RX480-E4 VT --- Arduino Digital Pin 2
    RX480-E4 ANT --- Looser coiled antenna
    ```

2.  **Receiver Pairing (CRUCIAL FIRST STEP!):**
    The RX480-E4 needs to "learn" the unique ID of your TX-118S transmitter. **Perform these steps *before* running your full sensor package code.**

    a.  **Power up the Receiver Arduino** (your sensor package Arduino).
    b.  **Clear Existing Data:** On the RX480-E4 module, locate the small **learning button**. Press it **8 times**. The small LED on the receiver will flash 7 times to confirm the data is cleared.
    c.  **Enter Learning Mode:** Press the **learning button** on the RX480-E4 module **once** (this sets it to "Momentary Mode," which is suitable for our continuous detection). The LED on the receiver will turn ON, indicating it's in learning mode.
    d.  **Send Signal from Transmitter:** While the receiver LED is ON, briefly power up your **Transmitter Arduino** (or if it's already running, it will immediately start sending pulses).
    e.  **Confirm Pairing:** The LED on the receiver should flash three times rapidly, indicating successful learning.

    Now, whenever the paired transmitter sends a signal, the `VT` pin on the receiver will go HIGH.

3.  **Receiver Code Integration:**
    The following updated code for your main sensor package Arduino includes the necessary variables, setup, and display logic for the RF receiver.

    ```cpp
    #include <Wire.h>
    #include <DFRobot_BMX160.h>
    #include <Adafruit_AHTX0.h>
    #include "rgb_lcd.h"
    #include "DFRobot_GNSS.h"
    #include <math.h>

    // --- Pin and Address Definitions ---
    const int NEXT_SCREEN_BUTTON_PIN = 4;
    const int CHANGE_COLOR_BUTTON_PIN = 5;
    const int RF_RECEIVER_VT_PIN = 2; // Connect RX480-E4 VT pin to Arduino Digital Pin 2 (interrupt capable)

    // --- Object Instantiation ---
    DFRobot_BMX160 bmx;
    Adafruit_AHTX0 aht;
    rgb_lcd lcd;
    DFRobot_GNSS_I2C gnss(&Wire, 0x20);

    // --- Global Variables ---
    int currentScreen = 0;
    int lastDisplayedScreen = -1;
    // Total screens: Welcome (0), Accel (1), Gyro (2), Mag (3), Environment (4), GPS (5), Max Accel (6), RF Signal (7)
    const int NUM_SCREENS = 8; // Increased for the new RF screen

    // Button state management - WITH INDEPENDENT DEBOUNCE TIMERS FOR EACH BUTTON
    bool nextButtonState = HIGH;
    bool lastNextButtonState = HIGH;
    unsigned long lastNextButtonDebounceTime = 0; // Independent debounce time for Next button

    bool colorButtonState = HIGH;
    bool lastColorButtonState = HIGH;
    unsigned long lastColorButtonDebounceTime = 0; // Independent debounce time for Color button
    unsigned long colorButtonPressStartTime = 0;    // Time when color button was first pressed down
    bool colorButtonLongPressed = false;            // Flag to prevent repeated resets during one long press
    const long longPressDuration = 2000;            // 2 seconds for long press

    unsigned long debounceDelay = 50; // Constant debounce delay for both buttons

    // LCD color state
    int currentColor = 0;
    const int NUM_COLORS = 5;
    byte colors[NUM_COLORS][3] = {
      {30, 144, 255}, {50, 205, 50}, {255, 215, 0}, {255, 69, 0}, {148, 0, 211}
    };

    // --- Global Variables for Timing (for Serial and LCD updates) ---
    unsigned long lastSerialPrintTime = 0;
    const long serialPrintInterval = 1000;

    unsigned long lastLcdUpdateTime = 0;
    const long lcdUpdateInterval = 500; // 500 milliseconds = 0.5 seconds for LCD updates

    // --- Global Variable for Max Acceleration ---
    float maxAccelMagnitude = 0.0; // Stores the maximum acceleration magnitude observed

    // --- Kalman Filter Parameters for Accelerometer Data ---
    const float Q_ACCEL = 0.001; // Process noise covariance. Tune as needed.
    const float R_ACCEL = 0.1;   // Measurement noise covariance. Tune as needed.

    // State variables for Kalman filter (estimated value and error covariance) for each axis
    float x_hat_accel_x = 0.0; float P_accel_x = 1.0;
    float x_hat_accel_y = 0.0; float P_accel_y = 1.0;
    float x_hat_accel_z = 0.0; float P_accel_z = 1.0;

    // --- RF Receiver Global Variables ---
    volatile unsigned int rfSignalCount = 0;      // Incremented by interrupt (volatile as it's modified in ISR)
    unsigned long lastRfMeasurementTime = 0;     // Last time we updated RF strength
    const long rfMeasurementInterval = 500;      // Measure RF signals every 500 ms
    int currentRfStrength = 0;                   // The 'signal strength' to display

    // --- Function Prototypes ---
    // Declare all functions used before their definitions to avoid "not declared in this scope" errors
    void kalmanFilter(float measuredValue, float &x_hat, float &P, const float Q, const float R);
    void handleButtons();
    void updateDisplay();
    void printDataToSerial();
    void displayWelcomeScreen();
    void displayAccelScreen();
    void displayGyroScreen();
    void displayMagScreen();
    void displayEnvironmentScreen();
    void displayGpsScreen();
    void displayMaxAccelScreen();
    void displayRfSignalScreen(); // New RF signal screen
    void displayError(String line1, String line2);
    void rfSignalInterrupt(); // Interrupt Service Routine for RF receiver

    // --- Main Setup Function ---
    void setup() {
      Serial.begin(115200);
      while (!Serial);
      Serial.println("--- Sensor Hub Initializing ---");

      Wire.begin();

      lcd.begin(16, 2);
      lcd.setRGB(colors[currentColor][0], colors[currentColor][1], colors[currentColor][2]);
      lcd.print("Sensor Hub");
      lcd.setCursor(0, 1);
      lcd.print("Initializing...");
      Serial.println("LCD Initialized.");
      delay(2000);

      if (bmx.begin()) {
        Serial.println("BMX160 Initialized.");
        // Read initial accelerometer data for Kalman filter initialization
        sBmx160SensorData_t initialAccel, initialGyro, initialMag;
        bmx.getAllData(&initialMag, &initialGyro, &initialAccel);
        x_hat_accel_x = initialAccel.x;
        x_hat_accel_y = initialAccel.y;
        x_hat_accel_z = initialAccel.z;
        Serial.println("Kalman filter initialized with first accel readings.");
      } else {
        Serial.println("Error: BMX160 not found!");
        displayError("BMX160 Error", "Check wiring");
        while (1); // Halt if BMX160 fails
      }
      bmx.wakeUp();

      if (aht.begin()) {
        Serial.println("AHT20 Initialized.");
      } else {
        Serial.println("Error: AHT20 not found!");
        displayError("AHT20 Error", "Check wiring");
        while (1); // Halt if AHT20 fails
      }

      while (!gnss.begin()) {
        Serial.println("Error: I2C GNSS not found!");
        displayError("GNSS Error", "Check wiring");
        delay(1000); // Wait and retry GNSS
      }
      Serial.println("I2C GNSS Initialized.");
      gnss.enablePower();
      gnss.setGnss(eGPS_BeiDou_GLONASS);

      // Configure buttons with internal pull-up resistors
      pinMode(NEXT_SCREEN_BUTTON_PIN, INPUT_PULLUP);
      pinMode(CHANGE_COLOR_BUTTON_PIN, INPUT_PULLUP);
      Serial.println("Buttons configured.");

      // Configure RF Receiver pin for interrupt
      pinMode(RF_RECEIVER_VT_PIN, INPUT); // The RX480-E4 VT pin outputs a HIGH/LOW signal
      // Attach an interrupt to the RF_RECEIVER_VT_PIN.
      // RISING: Trigger interrupt when VT goes from LOW to HIGH (valid packet detected)
      attachInterrupt(digitalPinToInterrupt(RF_RECEIVER_VT_PIN), rfSignalInterrupt, RISING);
      Serial.println("RF Receiver configured for interrupts.");

      Serial.println("--- Initialization Complete ---");
      lcd.clear();
    }

    // --- Main Loop ---
    void loop() {
      handleButtons(); // Handle button presses and debouncing

      // Measure RF signal strength periodically
      if (millis() - lastRfMeasurementTime >= rfMeasurementInterval) {
        lastRfMeasurementTime = millis();
        // Safely read and reset the volatile counter
        noInterrupts(); // Disable interrupts while reading/resetting rfSignalCount
        currentRfStrength = rfSignalCount;
        rfSignalCount = 0;
        interrupts(); // Re-enable interrupts
        //Serial.print("RF Signal Count: "); // Uncomment for Serial Debugging
        //Serial.println(currentRfStrength); // Uncomment for Serial Debugging
      }

      // Update LCD only if enough time has passed (0.5 seconds)
      if (millis() - lastLcdUpdateTime >= lcdUpdateInterval) {
        lastLcdUpdateTime = millis();
        updateDisplay();
      }

      // Print data to Serial Monitor only if enough time has passed (1 second)
      if (millis() - lastSerialPrintTime >= serialPrintInterval) {
        lastSerialPrintTime = millis();
        printDataToSerial();
      }
    }

    // --- Kalman Filter Function ---
    void kalmanFilter(float measuredValue, float &x_hat, float &P, const float Q, const float R) {
      P = P + Q; // Prediction update for error covariance
      float K = P / (P + R); // Kalman Gain
      x_hat = x_hat + K * (measuredValue - x_hat); // Measurement update for estimated state
      P = (1 - K) * P; // Measurement update for error covariance
    }

    // --- Function to Print All Sensor Data to Serial Monitor ---
    void printDataToSerial() {
      // Read all sensor data
      sBmx160SensorData_t accel, gyro, mag;
      bmx.getAllData(&mag, &gyro, &accel);

      // Apply Kalman filter to accelerometer data for Serial output
      kalmanFilter(accel.x, x_hat_accel_x, P_accel_x, Q_ACCEL, R_ACCEL);
      kalmanFilter(accel.y, x_hat_accel_y, P_accel_y, Q_ACCEL, R_ACCEL);
      kalmanFilter(accel.z, x_hat_accel_z, P_accel_z, Q_ACCEL, R_ACCEL);

      sensors_event_t humidity, temp;
      aht.getEvent(&humidity, &temp);

      // Print formatted data
      Serial.println("==============================================");
      Serial.println("       VEHICLE STATE MONITORING PACKAGE       ");
      Serial.println("==============================================");

      // IMU Data
      Serial.println("[IMU - BMX160]");
      Serial.print("   Accel (m/s^2) - Filtered: ");
      Serial.print("X: "); Serial.print(x_hat_accel_x, 2);
      Serial.print("\tY: "); Serial.print(x_hat_accel_y, 2);
      Serial.print("\tZ: "); Serial.println(x_hat_accel_z, 2);

      Serial.print("   Gyro (deg/s):  ");
      Serial.print("X: "); Serial.print(gyro.x, 2);
      Serial.print("\tY: "); Serial.print(gyro.y, 2);
      Serial.print("\tZ: "); Serial.println(gyro.z, 2);

      Serial.print("   Mag (uT):      ");
      Serial.print("X: "); Serial.print(mag.x, 2);
      Serial.print("\tY: "); Serial.print(mag.y, 2);
      Serial.print("\tZ: "); Serial.println(mag.z, 2);
      Serial.println();

      // Environment Data
      Serial.println("[Environment - AHT20]");
      Serial.print("   Temperature: "); Serial.print(temp.temperature, 1); Serial.println(" C");
      Serial.print("   Humidity:    "); Serial.print(humidity.relative_humidity, 1); Serial.println(" %");
      Serial.println();

      // GPS Data
      Serial.println("[GPS - DFRobot GNSS]");
      sLonLat_t lat = gnss.getLat();
      sLonLat_t lon = gnss.getLon();

      // Check for a valid satellite fix before printing GPS data
      if (lat.latitude == 0.0 && lon.lonitude == 0.0) {
        Serial.println("   Status: Searching for satellite fix...");
      } else {
        sTim_t timeData = gnss.getUTC();
        sTim_t dateData = gnss.getDate();

        Serial.print("   UTC Date/Time: ");
        Serial.print(dateData.year); Serial.print("/");
        if(dateData.month < 10) Serial.print("0"); Serial.print(dateData.month); Serial.print("/");
        if(dateData.date < 10) Serial.print("0"); Serial.print(dateData.date);
        Serial.print(" ");
        if(timeData.hour < 10) Serial.print("0"); Serial.print(timeData.hour); Serial.print(":");
        if(timeData.minute < 10) Serial.print("0"); Serial.print(timeData.minute); Serial.print(":");
        if(timeData.second < 10) Serial.print("0"); Serial.println(timeData.second);

        Serial.print("   Latitude:      "); Serial.println(lat.latitude, 6);
        Serial.print("   Longitude:   "); Serial.println(lon.lonitude, 6);
        Serial.print("   Satellites:  "); Serial.println(gnss.getNumSatUsed());
        Serial.print("   Altitude:    "); Serial.print(gnss.getAlt(), 2); Serial.println(" m");
        Serial.print("   Speed:       "); Serial.print(gnss.getSog(), 2); Serial.println(" knots");
      }
      Serial.println("==============================================\n\n");
    }


    // --- Function to Handle Button Presses ---
    void handleButtons() {
      // --- Handle NEXT_SCREEN_BUTTON_PIN ---
      bool currentNextButtonReading = digitalRead(NEXT_SCREEN_BUTTON_PIN);
      
      if (currentNextButtonReading != lastNextButtonState) {
        lastNextButtonDebounceTime = millis();
      }

      if ((millis() - lastNextButtonDebounceTime) > debounceDelay) {
        if (currentNextButtonReading != nextButtonState) {
          nextButtonState = currentNextButtonReading;
          
          if (nextButtonState == LOW) {
            currentScreen = (currentScreen + 1) % NUM_SCREENS;
          }
        }
      }
      lastNextButtonState = currentNextButtonReading;

      // --- Handle CHANGE_COLOR_BUTTON_PIN ---
      bool currentColorButtonReading = digitalRead(CHANGE_COLOR_BUTTON_PIN);
      
      if (currentColorButtonReading != lastColorButtonState) {
        lastColorButtonDebounceTime = millis();
        if (currentColorButtonReading == LOW) { // Button just pressed down
          colorButtonPressStartTime = millis();
          colorButtonLongPressed = false; // Reset long press flag
        }
      }

      if ((millis() - lastColorButtonDebounceTime) > debounceDelay) {
        if (currentColorButtonReading != colorButtonState) {
          colorButtonState = currentColorButtonReading;
          
          if (colorButtonState == LOW && !colorButtonLongPressed) {
            currentColor = (currentColor + 1) % NUM_COLORS;
            lcd.setRGB(colors[currentColor][0], colors[currentColor][1], colors[currentColor][2]);
          }
        }
      }
      lastColorButtonState = currentColorButtonReading;

      // Check for long press of color button (for resetting max accel)
      if (colorButtonState == LOW && !colorButtonLongPressed) {
        if (millis() - colorButtonPressStartTime >= longPressDuration) {
          maxAccelMagnitude = 0.0; // Reset max acceleration
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Max Accel Reset");
          lcd.setCursor(0,1);
          lcd.print("Hold for color");
          colorButtonLongPressed = true;
          lcd.setRGB(255, 165, 0); // Orange for reset feedback
          delay(1000);
          lcd.setRGB(colors[currentColor][0], colors[currentColor][1], colors[currentColor][2]);
        }
      }
    }

    // --- Function to Update the LCD Display ---
    void updateDisplay() {
      if (currentScreen != lastDisplayedScreen) {
        lcd.clear();
        lastDisplayedScreen = currentScreen;
      }

      switch (currentScreen) {
        case 0: displayWelcomeScreen(); break;
        case 1: displayAccelScreen(); break;
        case 2: displayGyroScreen(); break;
        case 3: displayMagScreen(); break;
        case 4: displayEnvironmentScreen(); break;
        case 5: displayGpsScreen(); break;
        case 6: displayMaxAccelScreen(); break;
        case 7: displayRfSignalScreen(); break; // Display RF signal screen
      }
    }

    // --- Display Functions for Each Screen ---
    void displayWelcomeScreen() {
      lcd.setCursor(2, 0);
      lcd.print("Sensor Hub");
      lcd.setCursor(0, 1);
      lcd.print("Press BTN1->Next");
    }

    void displayAccelScreen() {
      sBmx160SensorData_t accel, gyro, mag;
      bmx.getAllData(&mag, &gyro, &accel);

      kalmanFilter(accel.x, x_hat_accel_x, P_accel_x, Q_ACCEL, R_ACCEL);
      kalmanFilter(accel.y, x_hat_accel_y, P_accel_y, Q_ACCEL, R_ACCEL);
      kalmanFilter(accel.z, x_hat_accel_z, P_accel_z, Q_ACCEL, R_ACCEL);

      lcd.setCursor(0, 0);
      lcd.print("Accel (Filt)");
      lcd.setCursor(0, 1);
      lcd.print("X" + String(x_hat_accel_x, 1) + " Y" + String(x_hat_accel_y, 1) + " Z" + String(x_hat_accel_z, 1));
    }

    void displayGyroScreen() {
      sBmx160SensorData_t accel, gyro, mag;
      bmx.getAllData(&mag, &gyro, &accel);

      lcd.setCursor(0, 0);
      lcd.print("Gyro (deg/s)");
      lcd.setCursor(0, 1);
      lcd.print("X" + String(gyro.x, 1) + " Y" + String(gyro.y, 1) + " Z" + String(gyro.z, 1));
    }

    void displayMagScreen() {
      sBmx160SensorData_t accel, gyro, mag;
      bmx.getAllData(&mag, &gyro, &accel);

      lcd.setCursor(0, 0);
      lcd.print("Mag (uT)");
      lcd.setCursor(0, 1);
      lcd.print("X" + String(mag.x, 1) + " Y" + String(mag.y, 1) + " Z" + String(mag.z, 1));
    }

    void displayEnvironmentScreen() {
      sensors_event_t humidity, temp;
      aht.getEvent(&humidity, &temp);

      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(temp.temperature, 1);
      lcd.print((char)223); // Degree symbol
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("Humid: ");
      lcd.print(humidity.relative_humidity, 1);
      lcd.print("%");
    }

    void displayGpsScreen() {
      sLonLat_t lat = gnss.getLat();
      sLonLat_t lon = gnss.getLon();

      if (lat.latitude != 0.0 || lon.lonitude != 0.0) {
        lcd.setCursor(0, 0);
        lcd.print("Lat: ");
        lcd.print(lat.latitude, 4);

        lcd.setCursor(0, 1);
        lcd.print("Lng: ");
        lcd.print(lon.lonitude, 4);
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Searching for");
        lcd.setCursor(0, 1);
        lcd.print("satellites...");
      }
    }

    void displayMaxAccelScreen() {
      sBmx160SensorData_t accel, gyro, mag;
      bmx.getAllData(&mag, &gyro, &accel);

      kalmanFilter(accel.x, x_hat_accel_x, P_accel_x, Q_ACCEL, R_ACCEL);
      kalmanFilter(accel.y, x_hat_accel_y, P_accel_y, Q_ACCEL, R_ACCEL);
      kalmanFilter(accel.z, x_hat_accel_z, P_accel_z, Q_ACCEL, R_ACCEL);

      float currentAccelMagnitude = sqrt(pow(x_hat_accel_x, 2) + pow(x_hat_accel_y, 2) + pow(x_hat_accel_z, 2));

      if (currentAccelMagnitude > maxAccelMagnitude) {
        maxAccelMagnitude = currentAccelMagnitude;
      }

      lcd.setCursor(0, 0);
      lcd.print("Max Accel (m/s^2)");
      lcd.setCursor(0, 1);
      lcd.print(String(maxAccelMagnitude, 2));
    }

    // --- New RF Signal Strength Display Function ---
    void displayRfSignalScreen() {
      lcd.setCursor(0, 0);
      lcd.print("Radiation Level:");
      lcd.setCursor(0, 1);
      // Display the count. Higher count means closer/stronger signal.
      lcd.print(currentRfStrength);
      // Since rfMeasurementInterval is 500ms, multiply by 2 to get pulses per second
      lcd.print(" pulses/s"); 

      // You can add logic here to change LCD color based on strength
      if (currentRfStrength > 50) { // Example threshold for high radiation
        lcd.setRGB(255, 0, 0); // Red
      } else if (currentRfStrength > 10) { // Example threshold for medium radiation
        lcd.setRGB(255, 165, 0); // Orange
      } else {
        lcd.setRGB(colors[currentColor][0], colors[currentColor][1], colors[currentColor][2]); // Revert to current color
      }
    }

    // --- Utility Function to Display Errors ---
    void displayError(String line1, String line2) {
      lcd.clear();
      lcd.setRGB(255, 0, 0); // Red color for error
      lcd.setCursor(0, 0);
      lcd.print(line1);
      lcd.setCursor(0, 1);
      lcd.print(line2);
    }

    // --- Interrupt Service Routine for RF Receiver ---
    // This function is called every time the RF_RECEIVER_VT_PIN goes HIGH
    void rfSignalInterrupt() {
      rfSignalCount++; // Increment the counter
    }
    ```

---

### 3. Monitoring and Alert System

Develop software logic in Arduino IDE that:

-   Continuously monitors sensor data (vibration, orientation, temperature, humidity, **RF radiation**)
-   Detects anomalies based on predetermined thresholds
-   Provides real-time alerts via an LCD display, LED signals, or audible alerts

Example pseudo-logic:

```cpp
// Within your loop or a dedicated check function:
if (imuData.acceleration.magnitude > ACCEL_THRESHOLD ||
    temp.temperature > MAX_TEMP_THRESHOLD ||
    humidity.relative_humidity > MAX_HUMIDITY_THRESHOLD ||
    currentRfStrength > RADIATION_THRESHOLD) { // Check RF signal strength
    
    // Alert driver via buzzer, LCD, or LEDs
    alertDriver(); 
}

void alertDriver() {
    // Example: Flash LCD red and display a warning
    lcd.setRGB(255, 0, 0);
    lcd.setCursor(0,0);
    lcd.print("!!! ALERT !!!");
    lcd.setCursor(0,1);
    lcd.print("Anomaly Detected!");
    // You might also add a buzzer or external LED here
}