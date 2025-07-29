# ESP32 Multimeter

This project is an ESP32-based multimeter capable of measuring DC voltage and current, and calculating power. It uses an ADS1115 16-bit ADC for high-precision readings and a WCS1700 hall effect current sensor.

## Features

- Measures DC Voltage
- Measures DC Current (using WCS1700 sensor)
- Calculates Power in Watts
- Dynamic voltage calibration for improved accuracy across different voltage ranges.
- Outputs readings to the Serial Monitor.
- Includes example code for a voltage-only meter with an I2C LCD display.

## Hardware Requirements

- ESP32 Development Board
- Adafruit ADS1115 16-bit ADC Module
- WCS1700 Current Sensor
- Jumper wires

### Optional for Example Code:
- 16x2 I2C LCD Display

## Software & Libraries

- [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/)
- **Libraries:**
  - `Adafruit ADS1X15`
  - `Wire` (built-in)
  - `LiquidCrystal_I2C` (for the example code)

You can install the Adafruit and LiquidCrystal libraries through the Arduino IDE's Library Manager.

## How to Use

1.  **Wiring:**
    - Connect the ADS1115 ADC to the ESP32 via I2C (SDA, SCL, VCC, GND).
    - Connect the output of your voltage divider circuit to an analog input pin on the ADS1115 (A0 is used in the code).
    - Connect the output of the WCS1700 current sensor to another analog input pin on the ADS1115 (A1 is used in the code).
    - Power the sensors appropriately.
2.  **Upload Code:**
    - Open the `esp32_multi_meter.ino` file in the Arduino IDE.
    - Select your ESP32 board and COM port.
    - Upload the sketch.
3.  **View Output:**
    - Open the Serial Monitor and set the baud rate to `115200`.
    - You will see the voltage, current, and power readings updated every 500ms.

## Calibration

### Voltage (`voltage_measurement.h`)
The voltmeter uses a dynamic calibration system to improve accuracy. It calculates a calibration factor based on the measured voltage using a linear interpolation between two points.

- `MIN_VOLTAGE` / `MAX_VOLTAGE`: The expected range of voltages you will measure.
- `MIN_CAL_FACTOR` / `MAX_CAL_FACTOR`: These are the calibration multipliers at the maximum and minimum voltages, respectively.

To fine-tune the accuracy, you can adjust `MIN_CAL_FACTOR` and `MAX_CAL_FACTOR` in the `voltage_measurement.h` file. The example file `exmpale/voltage only code.ino` contains helper functions (`calibrateDynamicSystem` and `recalibrateDynamicSystem`) that can guide you through finding the optimal values for your specific components.

### Current (`wcs1700_current.h`)
The current sensor reading can be adjusted via the `CALIBRATION_FACTOR` constant in the `wcs1700_current.h` file. You may need to change this value by comparing the sensor's readings with a known-accurate multimeter.

## Example Code

An example sketch, `exmpale/voltage only code.ino`, is provided. This code demonstrates how to measure voltage and display it on a 16x2 I2C LCD. It also contains more detailed functions for testing and demonstrating the dynamic calibration system.
