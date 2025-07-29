/*
 * Arduino Sensor Hub
 * * Description:
 * This sketch reads data from multiple sensors, displays it on a Grove I2C RGB LCD,
 * and outputs a consolidated report to the Serial Monitor. It includes
 * RF signal reception using the RX480-E4 VT pin to simulate signal strength
 * by counting received pulses over time.
 * * Author: Tejas Soni
 * * Date: July 22, 2025 (Modified for RX480-E4 VT-only RF with simulated strength July 28, 2025)
 */

// --- Library Includes ---
#include <Wire.h>
#include <DFRobot_BMX160.h>
#include <Adafruit_AHTX0.h>
#include "rgb_lcd.h"
#include "DFRobot_GNSS.h"
#include <math.h> // For sqrt() and pow() for acceleration magnitude calculation

// --- Pin and Address Definitions ---
const int NEXT_SCREEN_BUTTON_PIN = 4;
const int CHANGE_COLOR_BUTTON_PIN = 5;

// RF Receiver Pin (RX480-E4) - Only VT pin will be used
// IMPORTANT: Digital Pin 2 is used because it supports external interrupts (INT0 on Uno R4 Minima)
const int RF_VT_PIN = 2;

// --- Object Instantiation ---
DFRobot_BMX160 bmx;
Adafruit_AHTX0 aht;
rgb_lcd lcd;
DFRobot_GNSS_I2C gnss(&Wire, 0x20);

// --- Global Variables ---
int currentScreen = 0;
int lastDisplayedScreen = -1;
// Total screens: Welcome (0), Accel (1), Gyro (2), Mag (3), Environment (4), GPS (5), Max Accel (6), RF Signal (7)
const int NUM_SCREENS = 8; // Increased by 1 for the new RF Signal screen

// Button state management - NOW WITH INDEPENDENT DEBOUNCE TIMERS FOR EACH BUTTON
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

// --- Global Variables for RF Reception (RX480-E4 VT-only with simulated strength) ---
volatile unsigned int rfPulseCount = 0;    // Incremented by interrupt (volatile as it's modified in ISR)
unsigned long lastRfMeasurementTime = 0;   // Last time we updated RF strength
const long rfMeasurementInterval = 1000;   // Measure RF signals every 1000 ms (1 second)
int rfSignalPercentage = 0;                // The calculated 'signal strength' (0-100%)
const int MAX_PULSES_PER_INTERVAL = 2;    // *** TUNE THIS VALUE ***
                                           // This is the expected max pulses in rfMeasurementInterval (1 second).
                                           // If your transmitter sends a pulse every ~100ms, then 10 pulses/sec is max.
                                           // Adjust this based on your transmitter's actual sending rate.

// --- Function Prototypes ---
// Declare ISR function before setup()
void rfPulseDetected();

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

  // Configure RF Receiver VT pin for interrupt
  pinMode(RF_VT_PIN, INPUT);
  // Attach an interrupt to the RF_VT_PIN. RISING means trigger when VT goes from LOW to HIGH.
  attachInterrupt(digitalPinToInterrupt(RF_VT_PIN), rfPulseDetected, RISING);
  Serial.println("RF Receiver VT pin configured with interrupt.");

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
    noInterrupts(); // Disable interrupts while reading/resetting rfPulseCount
    int currentPulses = rfPulseCount;
    rfPulseCount = 0;
    interrupts(); // Re-enable interrupts

    // Calculate percentage, capping at 100%
    rfSignalPercentage = map(currentPulses, 0, MAX_PULSES_PER_INTERVAL, 0, 100);
    if (rfSignalPercentage > 100) {
      rfSignalPercentage = 100; // Cap at 100%
    }
    // Serial.print("Pulses: "); Serial.print(currentPulses); // Uncomment for debugging
    // Serial.print(", Percentage: "); Serial.println(rfSignalPercentage); // Uncomment for debugging
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

// --- Interrupt Service Routine (ISR) for RF Receiver VT Pin ---
// This function is called every time the RF_VT_PIN goes from LOW to HIGH.
void rfPulseDetected() {
  rfPulseCount++; // Increment the pulse counter
}

// --- Function to Read RF Receiver VT Pin (now just for conceptual clarity, actual counting is via ISR) ---
// This function is no longer actively reading, the ISR handles the counting.
// It's kept for consistency with previous structure but its content is minimal.
void readRfReceiver() {
  // The rfPulseCount is updated by the ISR.
  // rfSignalPercentage is updated in the loop based on rfPulseCount.
  // No direct digitalRead of VT here for the main `rfSignalDetected` boolean.
  // We will infer "detected" if percentage is > 0.
}


// --- Function to Print All Sensor Data to Serial Monitor ---
void printDataToSerial() {
  // Read all sensor data
  sBmx160SensorData_t accel, gyro, mag;
  bmx.getAllData(&mag, &gyro, &accel);

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  // Print formatted data
  Serial.println("==============================================");
  Serial.println("          VEHICLE STATE MONITORING PACKAGE    ");
  Serial.println("==============================================");

  // IMU Data
  Serial.println("[IMU - BMX160]");
  Serial.print("    Accel (m/s^2): ");
  Serial.print("X: "); Serial.print(accel.x, 2);
  Serial.print("\tY: "); Serial.print(accel.y, 2);
  Serial.print("\tZ: "); Serial.println(accel.z, 2);

  Serial.print("    Gyro (deg/s):  ");
  Serial.print("X: "); Serial.print(gyro.x, 2);
  Serial.print("\tY: "); Serial.print(gyro.y, 2);
  Serial.print("\tZ: "); Serial.println(gyro.z, 2);

  Serial.print("    Mag (uT):      ");
  Serial.print("X: "); Serial.print(mag.x, 2);
  Serial.print("\tY: "); Serial.print(mag.y, 2);
  Serial.print("\tZ: "); Serial.println(mag.z, 2);
  Serial.println();

  // Environment Data
  Serial.println("[Environment - AHT20]");
  Serial.print("    Temperature: "); Serial.print(temp.temperature, 1); Serial.println(" C");
  Serial.print("    Humidity:    "); Serial.print(humidity.relative_humidity, 1); Serial.println(" %");
  Serial.println();

  // GPS Data
  Serial.println("[GPS - DFRobot GNSS]");
  sLonLat_t lat = gnss.getLat();
  sLonLat_t lon = gnss.getLon();

  // Check for a valid satellite fix before printing GPS data
  if (lat.latitude == 0.0 && lon.lonitude == 0.0) {
    Serial.println("    Status: Searching for satellite fix...");
  } else {
    sTim_t timeData = gnss.getUTC();
    sTim_t dateData = gnss.getDate();

    Serial.print("    UTC Date/Time: ");
    Serial.print(dateData.year); Serial.print("/");
    if(dateData.month < 10) Serial.print("0"); Serial.print(dateData.month); Serial.print("/");
    if(dateData.date < 10) Serial.print("0"); Serial.print(dateData.date);
    Serial.print(" ");
    if(timeData.hour < 10) Serial.print("0"); Serial.print(timeData.hour); Serial.print(":");
    if(timeData.minute < 10) Serial.print("0"); Serial.print(timeData.minute); Serial.print(":");
    if(timeData.second < 10) Serial.println(timeData.second);

    Serial.print("    Latitude:        "); Serial.println(lat.latitude, 6);
    Serial.print("    Longitude:     "); Serial.println(lon.lonitude, 6);
    Serial.print("    Satellites:    "); Serial.println(gnss.getNumSatUsed());
    Serial.print("    Altitude:      "); Serial.print(gnss.getAlt(), 2); Serial.println(" m");
    Serial.print("    Speed:         "); Serial.print(gnss.getSog(), 2); Serial.println(" knots");
  }
  Serial.println();

  // RF Data
  Serial.println("[RF Signal]");
  Serial.print("    Strength: "); Serial.print(rfSignalPercentage); Serial.println(" %");
  Serial.println("==============================================\n\n");
}


// --- Function to Handle Button Presses ---
void handleButtons() {
  // --- Handle NEXT_SCREEN_BUTTON_PIN ---
  bool currentNextButtonReading = digitalRead(NEXT_SCREEN_BUTTON_PIN);

  // If the button state has changed, reset its debounce timer
  if (currentNextButtonReading != lastNextButtonState) {
    lastNextButtonDebounceTime = millis();
  }

  // If the debounce time has passed
  if ((millis() - lastNextButtonDebounceTime) > debounceDelay) {
    // If the button state is now stable and different from the last stable state
    if (currentNextButtonReading != nextButtonState) {
      nextButtonState = currentNextButtonReading; // Update the stable button state

      // If the button is now pressed (LOW, due to INPUT_PULLUP and wired to GND)
      if (nextButtonState == LOW) {
        currentScreen = (currentScreen + 1) % NUM_SCREENS; // Advance to the next screen
      }
    }
  }
  lastNextButtonState = currentNextButtonReading; // Save the current raw reading for the next loop iteration

  // --- Handle CHANGE_COLOR_BUTTON_PIN ---
  bool currentColorButtonReading = digitalRead(CHANGE_COLOR_BUTTON_PIN);

  // If the button state has changed, reset its debounce timer and record press start time
  if (currentColorButtonReading != lastColorButtonState) {
    lastColorButtonDebounceTime = millis();
    if (currentColorButtonReading == LOW) { // Button just pressed down
      colorButtonPressStartTime = millis();
      colorButtonLongPressed = false; // Reset long press flag
    }
  }

  // Check for stable button state (short press for color change)
  if ((millis() - lastColorButtonDebounceTime) > debounceDelay) {
    if (currentColorButtonReading != colorButtonState) {
      colorButtonState = currentColorButtonReading; // Update the stable button state

      // If the button is now stably pressed (short press) AND not a long press
      if (colorButtonState == LOW && !colorButtonLongPressed) {
        currentColor = (currentColor + 1) % NUM_COLORS; // Change LCD color
        lcd.setRGB(colors[currentColor][0], colors[currentColor][1], colors[currentColor][2]);
      }
    }
  }
  lastColorButtonState = currentColorButtonReading; // Save the current raw reading for the next loop iteration

  // Check for long press of color button (for resetting max accel)
  // This check should happen continuously while the button is held down
  if (colorButtonState == LOW && !colorButtonLongPressed) { // If button is held down and not yet registered as long press
    if (millis() - colorButtonPressStartTime >= longPressDuration) {
      maxAccelMagnitude = 0.0; // Reset max acceleration
      lcd.clear(); // Provide visual feedback
      lcd.setCursor(0,0);
      lcd.print("Max Accel Reset");
      lcd.setCursor(0,1);
      lcd.print("Hold for color"); // Inform user about button function
      colorButtonLongPressed = true; // Set flag to prevent repeated resets during this press
      // Optionally, change color to indicate reset
      lcd.setRGB(255, 165, 0); // Orange for reset feedback
      delay(1000); // Show message for a moment
      lcd.setRGB(colors[currentColor][0], colors[currentColor][1], colors[currentColor][2]); // Revert to current color
    }
  }
}

// --- Function to Update the LCD Display ---
void updateDisplay() {
  // Clear LCD only if the screen has changed
  if (currentScreen != lastDisplayedScreen) {
    lcd.clear();
    lastDisplayedScreen = currentScreen;
  }

  // Call the appropriate display function based on currentScreen
  switch (currentScreen) {
    case 0: displayWelcomeScreen(); break;
    case 1: displayAccelScreen(); break; // New screen for Accelerometer
    case 2: displayGyroScreen(); break;  // New screen for Gyroscope
    case 3: displayMagScreen(); break;   // New screen for Magnetometer
    case 4: displayEnvironmentScreen(); break;
    case 5: displayGpsScreen(); break;
    case 6: displayMaxAccelScreen(); break; // New screen for Max Acceleration
    case 7: displayRfSignalScreen(); break; // New screen for RF Signal
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
  bmx.getAllData(&mag, &gyro, &accel); // Read all data, but only use accel

  lcd.setCursor(0, 0);
  lcd.print("Accel (m/s^2)");
  lcd.setCursor(0, 1);
  lcd.print("X" + String(accel.x, 1) + " Y" + String(accel.y, 1) + " Z" + String(accel.z, 1));
}

void displayGyroScreen() {
  sBmx160SensorData_t accel, gyro, mag;
  bmx.getAllData(&mag, &gyro, &accel); // Read all data, but only use gyro

  lcd.setCursor(0, 0);
  lcd.print("Gyro (deg/s)");
  lcd.setCursor(0, 1);
  lcd.print("X" + String(gyro.x, 1) + " Y" + String(gyro.y, 1) + " Z" + String(gyro.z, 1));
}

void displayMagScreen() {
  sBmx160SensorData_t accel, gyro, mag;
  bmx.getAllData(&mag, &gyro, &accel); // Read all data, but only use mag

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

  // If GPS has a valid fix, display coordinates
  if (lat.latitude != 0.0 || lon.lonitude != 0.0) { // Check if either is non-zero (simple fix check)
    lcd.setCursor(0, 0);
    lcd.print("Lat: ");
    lcd.print(lat.latitude, 4);

    lcd.setCursor(0, 1);
    lcd.print("Lng: ");
    lcd.print(lon.lonitude, 4);
  } else {
    // If no fix, display searching message
    lcd.clear(); // Clear the screen to show the searching message properly
    lcd.setCursor(0, 0);
    lcd.print("Searching for");
    lcd.setCursor(0, 1);
    lcd.print("satellites...");
  }
}

void displayMaxAccelScreen() {
  sBmx160SensorData_t accel, gyro, mag;
  bmx.getAllData(&mag, &gyro, &accel); // Read all data, but only use accel

  // Calculate the magnitude of the acceleration vector
  float currentAccelMagnitude = sqrt(pow(accel.x, 2) + pow(accel.y, 2) + pow(accel.z, 2));

  // Update maxAccelMagnitude if the current magnitude is higher
  if (currentAccelMagnitude > maxAccelMagnitude) {
    maxAccelMagnitude = currentAccelMagnitude;
  }

  lcd.setCursor(0, 0);
  lcd.print("Max Accel (m/s^2)");
  lcd.setCursor(0, 1);
  lcd.print(String(maxAccelMagnitude, 2)); // Display max acceleration with 2 decimal places
}

void displayRfSignalScreen() {
  lcd.setCursor(0, 0);
  lcd.print("RF Signal:"); // Concise title

  lcd.setCursor(0, 1);
  lcd.print("Strength: ");
  lcd.print(rfSignalPercentage);
  lcd.print("%");

  // Optional: Change LCD color based on signal strength
  if (rfSignalPercentage > 75) {
    lcd.setRGB(0, 255, 0); // Green for strong signal
  } else if (rfSignalPercentage > 25) {
    lcd.setRGB(255, 165, 0); // Orange for medium signal
  } else if (rfSignalPercentage > 0) {
    lcd.setRGB(255, 0, 0); // Red for weak/intermittent signal
  } else {
    lcd.setRGB(colors[currentColor][0], colors[currentColor][1], colors[currentColor][2]); // Revert to current color if no signal
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