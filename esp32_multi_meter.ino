#include "voltage_measurement.h"
#include "wcs1700_current.h"

VoltageMeasurement voltmeter;
WCS1700Current currentSensor; // Uses channel 1 by default

void setup() {
  Serial.begin(115200);
  voltmeter.begin();
  currentSensor.begin();
}

void loop() {
  float voltage = voltmeter.readVoltage();
  float current = currentSensor.readCurrent();
  float power = voltage * current;
  
  Serial.print("V: ");
  Serial.print(voltage, 3);
  Serial.print("V | I: ");
  Serial.print(current, 3);
  Serial.print("A | P: ");
  Serial.print(power, 2);
  Serial.println("W");
  
  delay(500);
}