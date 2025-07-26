#ifndef VOLTAGE_MEASUREMENT_H
#define VOLTAGE_MEASUREMENT_H

#include <Wire.h>
#include <Adafruit_ADS1X15.h>

class VoltageMeasurement {
private:
    Adafruit_ADS1115 ads;
    
    // Constants
    static const float VOLTAGE_DIVIDER_RATIO;
    static const float MIN_VOLTAGE;
    static const float MAX_VOLTAGE;
    static const float MIN_CAL_FACTOR;
    static const float MAX_CAL_FACTOR;
    static const float CAL_SLOPE;
    
public:
    VoltageMeasurement();
    bool begin();
    
    // Essential functions
    float readVoltage();
    float getCalibrationFactor();
    int16_t getAnalogValue();
    
private:
    float calculateCalibrationFactor(float voltage);
};

// Constants
const float VoltageMeasurement::VOLTAGE_DIVIDER_RATIO = 11.0;
const float VoltageMeasurement::MIN_VOLTAGE = 0.0;
const float VoltageMeasurement::MAX_VOLTAGE = 45.0;
const float VoltageMeasurement::MIN_CAL_FACTOR = 0.91;
const float VoltageMeasurement::MAX_CAL_FACTOR = 1.011;
const float VoltageMeasurement::CAL_SLOPE = (MIN_CAL_FACTOR - MAX_CAL_FACTOR) / (MAX_VOLTAGE - MIN_VOLTAGE);

// Implementation
inline VoltageMeasurement::VoltageMeasurement() {}

inline bool VoltageMeasurement::begin() {
    Wire.begin();
    if (!ads.begin()) return false;
    ads.setGain(GAIN_ONE);
    return true;
}

inline int16_t VoltageMeasurement::getAnalogValue() {
    return ads.readADC_SingleEnded(0);
}

inline float VoltageMeasurement::calculateCalibrationFactor(float voltage) {
    if (voltage < MIN_VOLTAGE) voltage = MIN_VOLTAGE;
    if (voltage > MAX_VOLTAGE) voltage = MAX_VOLTAGE;
    return MAX_CAL_FACTOR + (CAL_SLOPE * voltage);
}

inline float VoltageMeasurement::readVoltage() {
    // Get raw reading
    int16_t adc = getAnalogValue();
    float adcVoltage = ads.computeVolts(adc);
    float rawVoltage = adcVoltage * VOLTAGE_DIVIDER_RATIO;
    
    // Apply calibration
    float calFactor = calculateCalibrationFactor(rawVoltage);
    return rawVoltage * calFactor;
}

inline float VoltageMeasurement::getCalibrationFactor() {
    // Get current voltage for calibration factor
    int16_t adc = getAnalogValue();
    float adcVoltage = ads.computeVolts(adc);
    float rawVoltage = adcVoltage * VOLTAGE_DIVIDER_RATIO;
    return calculateCalibrationFactor(rawVoltage);
}

#endif