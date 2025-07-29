#ifndef WCS1700_CURRENT_H
#define WCS1700_CURRENT_H

#include <Wire.h>
#include <Adafruit_ADS1X15.h>

class WCS1700Current {
private:
    Adafruit_ADS1115 ads;
    uint8_t channel;
    
    // WCS1700 Constants
    static const float SENSITIVITY;        // 40mV/A
    static const float ZERO_CURRENT_VOLTAGE; // 2.5V at 0A
    static const float MAX_CURRENT;        // ±35A
    static const float CALIBRATION_FACTOR; // Adjust as needed
    
public:
    WCS1700Current(uint8_t adcChannel = 1);
    bool begin();
    
    // Essential functions
    float readCurrent();
    float getCalibrationFactor();
    int16_t getAnalogValue();
    
private:
    float calculateCurrent(float voltage);
};

// Constants
const float WCS1700Current::SENSITIVITY = 0.066;           // 40mV/A
const float WCS1700Current::ZERO_CURRENT_VOLTAGE = 0;    // 2.5V at 0A
const float WCS1700Current::MAX_CURRENT = 75.0;           // ±35A
const float WCS1700Current::CALIBRATION_FACTOR = 1.0;     // Adjust as needed

// Implementation
inline WCS1700Current::WCS1700Current(uint8_t adcChannel) : channel(adcChannel) {}

inline bool WCS1700Current::begin() {
    Wire.begin();
    if (!ads.begin()) return false;
    ads.setGain(GAIN_ONE); // ±4.096V range
    return true;
}

inline int16_t WCS1700Current::getAnalogValue() {
    return ads.readADC_SingleEnded(channel);
}

inline float WCS1700Current::calculateCurrent(float voltage) {
    // Current = (Voltage - ZeroCurrentVoltage) / Sensitivity
    float current = (voltage - ZERO_CURRENT_VOLTAGE) / SENSITIVITY;
    return current * CALIBRATION_FACTOR;
}

inline float WCS1700Current::readCurrent() {
    int16_t adc = getAnalogValue();
    float voltage = ads.computeVolts(adc);
    return calculateCurrent(voltage);
}

inline float WCS1700Current::getCalibrationFactor() {
    return CALIBRATION_FACTOR;
}

#endif