#include "voltage_measurement.h"

VoltageMeasurement voltmeter;

void setup() {
  Serial.begin(115200);
  voltmeter.begin();
}

void loop() {
  float voltage = voltmeter.readVoltage();
  float calFactor = voltmeter.getCalibrationFactor();
  int16_t analogVal = voltmeter.getAnalogValue();
  
  Serial.print("Voltage: ");
  Serial.print(voltage, 3);
  Serial.print("V | Cal: ");
  Serial.print(calFactor, 4);
  Serial.print(" | ADC: ");
  Serial.println(analogVal);
  
  delay(500);
}