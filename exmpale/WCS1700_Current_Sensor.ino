#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <LiquidCrystal_I2C.h>

//used ads1x15 A1 pin
// Hardware setup
Adafruit_ADS1115 ads;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// WCS1700 Specifications (from datasheet)
const float WCS1700_ZERO_CURRENT_VOLTAGE = 2.5;  // 2.5V at 0A
const float WCS1700_SENSITIVITY = 0.040;         // 40mV/A = 0.040V/A
const int CURRENT_CHANNEL = 1;                   // ADS1115 A1 channel
const int NUM_SAMPLES = 1;                      // Number of samples to average

// Calibration variables
float zeroOffset = 0.0;        // Measured zero current offset
bool isCalibrated = false;     // Calibration flag

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // Initialize ADS1115
  if (!ads.begin()) {
    Serial.println("ADS1115 initialization failed!");
    while (1);
  }
  ads.setGain(GAIN_ONE); // ±4.096V range
  
  // Initialize LCD
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("WCS1700 Current");
  
  Serial.println("WCS1700 Current Sensor");
  Serial.println("======================");
  Serial.println("Specifications:");
  Serial.println("- Zero current: 2.5V");
  Serial.println("- Sensitivity: 40mV/A");
  Serial.println("- Range: ±35A");
  
  // Auto-calibration
  calibrateZeroCurrent();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WCS1700 Ready");
  delay(1000);
}

void loop() {
  float current = readCurrent();
  
  // Serial output
  Serial.print("Current: ");
  Serial.print(current, 3);
  Serial.print(" A | Voltage: ");
  Serial.print(readVoltage(), 3);
  Serial.print(" V | Offset: ");
  Serial.print(zeroOffset, 3);
  Serial.println(" A");
  
  // LCD display
  lcd.setCursor(0, 0);
  lcd.print("I: ");
  lcd.print(current, 2);
  lcd.print(" A      ");
  
  lcd.setCursor(0, 1);
  if (abs(current) < 0.05) {
    lcd.print("Status: ZERO    ");
  } else if (abs(current) > 20.0) {
    lcd.print("Status: HIGH    ");
  } else {
    lcd.print("Status: OK      ");
  }
  
  delay(250);
}

// Read voltage from WCS1700
float readVoltage() {
  float totalVoltage = 0;
  
  // Take multiple readings for accuracy
  for (int i = 0; i < NUM_SAMPLES; i++) {
    int16_t adc = ads.readADC_SingleEnded(CURRENT_CHANNEL);
    float voltage = ads.computeVolts(adc);
    totalVoltage += voltage;
    delay(2);
  }
  
  return totalVoltage / NUM_SAMPLES;
}

// Convert voltage to current using WCS1700 formula
float voltageToCurrent(float voltage) {
  // WCS1700 Formula: Current = (Voltage - 2.5V) / 0.040V/A
  return (voltage - WCS1700_ZERO_CURRENT_VOLTAGE) / WCS1700_SENSITIVITY;
}

// Read current with calibration
float readCurrent() {
  float voltage = readVoltage();
  float current = voltageToCurrent(voltage);
  
  // Apply zero offset calibration
  if (isCalibrated) {
    current -= zeroOffset;
  }
  
  return current;
}

// Calibrate zero current (no load condition)
void calibrateZeroCurrent() {
  Serial.println("\nCalibrating zero current...");
  Serial.println("Ensure NO current is flowing through sensor!");
  
  lcd.setCursor(0, 1);
  lcd.print("Calibrating...  ");
  
  delay(3000); // Wait for stable reading
  
  // Measure zero current offset
  float voltage = readVoltage();
  zeroOffset = voltageToCurrent(voltage);
  isCalibrated = true;
  
  Serial.print("Zero calibration complete.");
  Serial.print(" Measured voltage: ");
  Serial.print(voltage, 4);
  Serial.print("V, Offset: ");
  Serial.print(zeroOffset, 4);
  Serial.println("A");
  
  lcd.setCursor(0, 1);
  lcd.print("Cal: ");
  lcd.print(zeroOffset, 3);
  lcd.print("A   ");
  delay(2000);
}

// Manual recalibration function
void recalibrate() {
  Serial.println("\n=== Manual Recalibration ===");
  calibrateZeroCurrent();
}

// Test different current levels (for validation)
void testCurrentLevels() {
  Serial.println("\n=== WCS1700 Test Results ===");
  
  for (int test = 0; test < 10; test++) {
    float voltage = readVoltage();
    float rawCurrent = voltageToCurrent(voltage);
    float calibratedCurrent = readCurrent();
    
    Serial.print("Test ");
    Serial.print(test + 1);
    Serial.print(": V=");
    Serial.print(voltage, 4);
    Serial.print("V, Raw I=");
    Serial.print(rawCurrent, 4);
    Serial.print("A, Cal I=");
    Serial.print(calibratedCurrent, 4);
    Serial.println("A");
    
    delay(500);
  }
  Serial.println("============================");
}

// Print sensor specifications
void printSpecs() {
  Serial.println("\n=== WCS1700 Specifications ===");
  Serial.print("Zero Current Voltage: ");
  Serial.print(WCS1700_ZERO_CURRENT_VOLTAGE);
  Serial.println("V");
  Serial.print("Sensitivity: ");
  Serial.print(WCS1700_SENSITIVITY * 1000);
  Serial.println("mV/A");
  Serial.print("Current Range: ±");
  Serial.print(2.096 / WCS1700_SENSITIVITY); // Based on ADS1115 range
  Serial.println("A");
  Serial.print("Resolution: ~");
  Serial.print((ads.computeVolts(1) / WCS1700_SENSITIVITY) * 1000);
  Serial.println("mA");
  
  if (isCalibrated) {
    Serial.print("Zero Offset: ");
    Serial.print(zeroOffset, 4);
    Serial.println("A");
    Serial.print("Actual Zero Voltage: ");
    Serial.print(WCS1700_ZERO_CURRENT_VOLTAGE + (zeroOffset * WCS1700_SENSITIVITY), 4);
    Serial.println("V");
  }
  Serial.println("===============================");
}

// Uncomment these lines in loop() for testing:
// recalibrate(); delay(10000); return;        // Manual recalibration
// testCurrentLevels(); delay(10000); return;  // Test readings
// printSpecs(); delay(10000); return;         // Show specifications