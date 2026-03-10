/**
 * 7Semi STS3x Temperature Sensor Library
 *
 * - Supports single shot measurement
 * - Supports periodic measurement
 * - Compatible with Arduino / ESP32
 * - Based on Sensirion STS3x datasheet
 */
#ifndef _7SEMI_STS3X_H_
#define _7SEMI_STS3X_H_

#include <Arduino.h>
#include <Wire.h>

#define STS3X_I2C_ADDR             0x4A

#define STS3X_READ_STATUS          0xF32D
#define STS3X_CLEAR_STATUS         0x3041
#define STS3X_I2C_SOFT_RESET       0x30A2
#define STS3X_STOP_MEASUREMENT     0x3093
#define STS3X_BLOCKING_MEASUREMENT 0xE000



// Repeatability enum
enum Repeatability {
    REPEATABILITY_LOW = 0,
    REPEATABILITY_MEDIUM,
    REPEATABILITY_HIGH,
    REPEATABILITY_COUNT
};

// Measurement rates for periodic measurements (MPS)
enum MeasurementRates {
    MPS_0_5 = 0,
    MPS_1,
    MPS_2,
    MPS_4,
    MPS_10,
    MPS_COUNT
};

class STS3x_7Semi {
public:
    STS3x_7Semi();

/**
 * Initialize device
 *
 * - address : I2C address
 * - port : Wire interface
 * - clock : I2C clock speed
 * - sda/scl : ESP32 optional pins
 * - returns : true if sensor responds on I2C
 */
bool begin(uint8_t i2cAddress = STS3X_I2C_ADDR,
           TwoWire &i2cPort = Wire,
           uint32_t i2cClock = 100000,
           uint8_t sda = 255,
           uint8_t scl = 255);

/**
 * Read temperature using high precision single shot
 *
 * - Uses highest repeatability
 * - Clock stretching disabled
 * - Returns NAN if measurement fails
 *
 * - returns : temperature in Celsius
 */
float readTemperature();

/**
 * Perform single shot temperature measurement
 *
 * - repeat : measurement precision level
 * - clockStretch : enable clock stretching mode
 * - temperature : output value in Celsius
 *
 * - returns : true if measurement successful
 */
bool measureSingleShot(Repeatability repeat, bool clockStretch, float &temperature);
   

/**
 * Start continuous temperature measurement
 *
 * - repeat : measurement precision level
 * - rate : measurements per second
 *
 * - Sensor will continuously update internal buffer
 */
bool startPeriodicMeasurement(Repeatability repeat, MeasurementRates rate);

/**
 * Read measurement from periodic mode
 *
 * - Blocks until measurement interval elapsed
 * - Reads latest sensor sample
 *
 * - temperature : output Celsius value
 */
bool blockingReadMeasurement(float &temperature);

/**
 * Stop periodic measurement mode
 *
 * - Sensor returns to idle state
 */
bool stopMeasurement();

/**
 * Perform sensor soft reset
 *
 * - Clears internal state
 * - Sensor reboots internally
 */
bool softReset();

/**
 * Read sensor status register
 *
 * - status : 16-bit sensor status flags
 *
 * - returns : true if read successful
 */
bool readStatusRegister(uint16_t &status) ;

/**
 * Clear sensor status register
 *
 * - Resets alert and error flags
 */
bool clearStatusRegister();

private:
    TwoWire *i2c;
    uint8_t address;
    MeasurementRates mps = MPS_1;

    bool sendCommand(uint16_t cmd, uint8_t *data = nullptr, size_t len = 0, uint16_t timeout = 0);

    float getTemperatureC(uint16_t raw);

    // Command Array
    static const uint16_t SINGLE_SHOT_COMMANDS[2][REPEATABILITY_COUNT];
    static const uint16_t PERIODIC_COMMANDS[MPS_COUNT][REPEATABILITY_COUNT];
    static const uint16_t MPS_DELAY[MPS_COUNT];

    /**
     * Sensirion getCRC calculation
     *
     * - Polynomial : 0x31
     * - Initial value : 0xFF
     * - Used for measurement frame validation
     *
     * - data : pointer to buffer
     * - len : number of bytes
     */
    uint8_t getCRC(const uint8_t *data, uint8_t len);
};

#endif