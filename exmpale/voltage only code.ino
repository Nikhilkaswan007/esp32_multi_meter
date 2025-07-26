#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <LiquidCrystal_I2C.h>

// Initialize ADS1115 (16-bit ADC)
Adafruit_ADS1115 ads;

// Initialize LCD (address 0x27 for most I2C LCD modules, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

const float VOLTAGE_DIVIDER_RATIO = 11.0; // For 100k:10k divider
const float ADS1115_VOLTAGE_RANGE = 4.096; // Gain = 1 (±4.096V)

// Dynamic calibration system
const float MIN_VOLTAGE = 0.0;       // Minimum voltage
const float MAX_VOLTAGE = 45.0;      // Maximum voltage
const float MIN_CAL_FACTOR = 0.91;  // Calibration factor at 45V
const float MAX_CAL_FACTOR = 1.011;  // Calibration factor at 0V

// Slope (calculated for linear interpolation)
const float CAL_SLOPE = (MIN_CAL_FACTOR - MAX_CAL_FACTOR) / (MAX_VOLTAGE - MIN_VOLTAGE);


// Function to calculate calibration factor based on voltage
float getDynamicCalibrationFactor(float voltage) {
  // Clamp voltage to valid range
  if (voltage < MIN_VOLTAGE) voltage = MIN_VOLTAGE;
  if (voltage > MAX_VOLTAGE) voltage = MAX_VOLTAGE;
  
  // Linear interpolation: factor = MAX_CAL_FACTOR + slope * voltage
  float factor = MAX_CAL_FACTOR + (CAL_SLOPE * voltage);
  
  return factor;
}

// Function to get voltage range description for display
String getVoltageRangeDescription(float voltage) {
  if (voltage < 1.0) return "Ultra Low";
  else if (voltage < 5.0) return "Low Range";
  else if (voltage < 10.0) return "Mid-Low";
  else if (voltage < 15.0) return "Mid Range";
  else if (voltage < 20.0) return "Mid-High";
  else return "High Range";
}

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C communication
  Wire.begin();
  
  // Initialize LCD
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("ESP32 Voltmeter");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  // Initialize ADS1115
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS1115!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ADS1115 Error!");
    while (1);
  }
  
  // Set gain to ±4.096V (default)
  ads.setGain(GAIN_ONE);
  
  Serial.println("ADS1115 initialized successfully!");
  Serial.println("\n=== Dynamic Calibration System ===");
  Serial.print("Calibration factor range: ");
  Serial.print(MAX_CAL_FACTOR, 3);
  Serial.print(" (at 0V) to ");
  Serial.print(MIN_CAL_FACTOR, 3);
  Serial.println(" (at 25V)");
  Serial.print("Linear slope: ");
  Serial.println(CAL_SLOPE, 6);
  Serial.println("Factor decreases smoothly as voltage increases");
  Serial.println("===================================\n");
  
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dynamic Cal Mode");
}

void loop() {
  // Uncomment one of these lines for testing:
  // calibrateDynamicSystem(); delay(5000); return;     // Dynamic calibration test
  // calibrateVoltmeter(); delay(5000); return;         // Calibration info
  // recalibrateDynamicSystem(); delay(5000); return;   // Recalibration helper
  
  // Take multiple readings for better accuracy
  float totalVoltage = 0;
  int numReadings = 5;
  
  // First pass: get rough voltage to determine calibration factor
  int16_t roughAdc = ads.readADC_SingleEnded(0);
  float roughAdcVoltage = ads.computeVolts(roughAdc);
  float roughVoltage = roughAdcVoltage * VOLTAGE_DIVIDER_RATIO;
  
  // Get dynamic calibration factor for this voltage
  float calibrationFactor = getDynamicCalibrationFactor(roughVoltage);
  
  // Now take accurate readings with proper calibration
  for (int i = 0; i < numReadings; i++) {
    int16_t adc0 = ads.readADC_SingleEnded(0);
    float adcVoltage = ads.computeVolts(adc0);
    float inputVoltage = adcVoltage * VOLTAGE_DIVIDER_RATIO * calibrationFactor;
    totalVoltage += inputVoltage;
    delay(10);
  }
  
  float averageVoltage = totalVoltage / numReadings;
  String rangeDesc = getVoltageRangeDescription(averageVoltage);
  
  // Display on Serial Monitor with dynamic calibration info
  Serial.print("Voltage: ");
  Serial.print(averageVoltage, 3);
  Serial.print("V | Cal Factor: ");
  Serial.print(calibrationFactor, 4);
  Serial.print(" | Range: ");
  Serial.println(rangeDesc);
  
  // Display on LCD - Line 1: Voltage, Line 2: Cal Factor
  lcd.setCursor(0, 0);
  lcd.print("                "); // Clear line
  lcd.setCursor(0, 0);
  
  if (averageVoltage < 10.0) {
    lcd.print(averageVoltage, 3);
    lcd.print("V");
  } else {
    lcd.print(averageVoltage, 2);
    lcd.print("V");
  }
  
  // Show status indicator
  if (averageVoltage > 23.0) {
    lcd.setCursor(11, 0);
    lcd.print(" HIGH");
  } else if (averageVoltage < 0.5) {
    lcd.setCursor(11, 0);
    lcd.print(" LOW ");
  } else {
    lcd.setCursor(11, 0);
    lcd.print("     ");
  }
  
  // Display calibration factor on second line
  lcd.setCursor(0, 1);
  lcd.print("                "); // Clear line
  lcd.setCursor(0, 1);
  lcd.print("Cal:");
  lcd.print(calibrationFactor, 3);
  lcd.print(" ");
  lcd.print(rangeDesc.substring(0, 8)); // Show first 8 chars of range
  
  delay(500); // Update every 500ms
}

// Function to test and demonstrate the dynamic calibration system
void calibrateDynamicSystem() {
  Serial.println("\n=== Dynamic Calibration System Test ===");
  Serial.println("Voltage | Expected Cal Factor | Formula");
  Serial.println("--------|---------------------|--------");
  
  // Show calibration factors at different voltages
  for (float testVolt = 0; testVolt <= 25; testVolt += 2.5) {
    float factor = getDynamicCalibrationFactor(testVolt);
    Serial.print(testVolt, 1);
    Serial.print("V     | ");
    Serial.print(factor, 4);
    Serial.print("             | ");
    Serial.print(MAX_CAL_FACTOR, 3);
    Serial.print(" + (");
    Serial.print(CAL_SLOPE, 6);
    Serial.print(" * ");
    Serial.print(testVolt, 1);
    Serial.println(")");
  }
  
  Serial.println("=========================================");
  Serial.println("Your calibration points:");
  Serial.print("At 0V: Factor = ");
  Serial.println(getDynamicCalibrationFactor(0), 4);
  Serial.print("At 25V: Factor = ");
  Serial.println(getDynamicCalibrationFactor(25), 4);
  Serial.println("Factor smoothly decreases as voltage increases!");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dynamic Test");
  lcd.setCursor(0, 1);
  lcd.print("Check Serial");
  delay(3000);
}

// Function to calibrate the multimeter using the dynamic system
void calibrateVoltmeter() {
  Serial.println("=== Dynamic Calibration System Setup ===");
  Serial.println("This system uses linear interpolation between two points:");
  Serial.print("Low end (0V): Factor = ");
  Serial.println(MAX_CAL_FACTOR, 3);
  Serial.print("High end (25V): Factor = ");
  Serial.println(MIN_CAL_FACTOR, 3);
  Serial.println("\nTo recalibrate, measure at two known voltages:");
  Serial.println("1. Apply ~1V and note the reading");
  Serial.println("2. Apply ~20V and note the reading");
  Serial.println("3. Update MAX_CAL_FACTOR and MIN_CAL_FACTOR in code");
  Serial.println("==========================================\n");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dynamic Cal");
  lcd.setCursor(0, 1);
  lcd.print("Check Serial");
  delay(3000);
}

// Function to help you recalibrate the dynamic system
void recalibrateDynamicSystem() {
  Serial.println("\n=== Dynamic System Recalibration Helper ===");
  Serial.println("Step 1: Apply a LOW voltage (1-3V)");
  Serial.println("Step 2: Note the displayed voltage vs actual");
  Serial.println("Step 3: Apply a HIGH voltage (20-24V)");  
  Serial.println("Step 4: Note the displayed voltage vs actual");
  Serial.println("\nThen update these values in code:");
  Serial.println("MAX_CAL_FACTOR = actualLowV / displayedLowV");
  Serial.println("MIN_CAL_FACTOR = actualHighV / displayedHighV");
  Serial.println("===========================================\n");
  
  // Show current readings for reference
  Serial.println("Current reading (apply your test voltage):");
  
  for (int i = 0; i < 10; i++) {
    int16_t adc0 = ads.readADC_SingleEnded(0);
    float adcVoltage = ads.computeVolts(adc0);
    float rawVoltage = adcVoltage * VOLTAGE_DIVIDER_RATIO;
    float currentFactor = getDynamicCalibrationFactor(rawVoltage);
    float calibratedVoltage = rawVoltage * currentFactor;
    
    Serial.print("Raw: ");
    Serial.print(rawVoltage, 3);
    Serial.print("V | Factor: ");
    Serial.print(currentFactor, 4);
    Serial.print(" | Calibrated: ");
    Serial.print(calibratedVoltage, 3);
    Serial.println("V");
    
    lcd.setCursor(0, 0);
    lcd.print("Recal Test");
    lcd.setCursor(0, 1);
    lcd.print(calibratedVoltage, 3);
    lcd.print("V        ");
    
    delay(1000);
  }
}