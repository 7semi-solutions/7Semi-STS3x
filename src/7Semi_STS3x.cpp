#include "7Semi_STS3x.h"

/**
 * Single shot measurement commands
 *
 * - Index 0 : Clock stretching disabled
 * - Index 1 : Clock stretching enabled
 *
 * - Columns represent repeatability level
 *   - LOW
 *   - MEDIUM
 *   - HIGH
 */
const uint16_t STS3x_7Semi::SINGLE_SHOT_COMMANDS[2][REPEATABILITY_COUNT] = {
   // LOW     MEDIUM   HIGH
    {0x2416, 0x240B, 0x2400}, //Clock stretching disabled
    {0x2C10, 0x2C0D, 0x2C06}  //Clock stretching enabled
};

/**
 * Periodic measurement commands
 *
 * - Rows represent measurement rate
 * - Columns represent repeatability
 *
 * Rates:
 * - 0.5 MPS
 * - 1 MPS
 * - 2 MPS
 * - 4 MPS
 * - 10 MPS
 */
const uint16_t STS3x_7Semi::PERIODIC_COMMANDS[MPS_COUNT][REPEATABILITY_COUNT] = {
  // LOW     MEDIUM   HIGH
    {0x202F, 0x2024, 0x2032}, // 0.5 MPS
    {0x212D, 0x2126, 0x2130}, // 1 MPS
    {0x222B, 0x2220, 0x2236}, // 2 MPS
    {0x2329, 0x2322, 0x2334}, // 4 MPS
    {0x272A, 0x2721, 0x2737}  // 10 MPS
};

/**
 * Delay required between periodic reads
 *
 * - Values in milliseconds
 * - Based on measurement rate
 */
const uint16_t STS3x_7Semi::MPS_DELAY[MPS_COUNT] = {2000, 1000, 500, 250, 95};

STS3x_7Semi::STS3x_7Semi(){}

/**
 * Initialize device
 *
 * - address : I2C address
 * - port : Wire interface
 * - clock : I2C clock speed
 * - sda/scl : ESP32 optional pins
 *
 * - returns : true if device responds on I2C bus
 */
bool STS3x_7Semi::begin(uint8_t i2cAddress, TwoWire &i2cPort, uint32_t i2cClock, uint8_t sda, uint8_t scl)
{
    i2c = &i2cPort;
    address = i2cAddress;

#ifdef ARDUINO_ARCH_ESP32
    if (sda != 255 && scl != 255)
        i2c->begin(sda, scl);
    else
        i2c->begin();
#else
    i2c->begin();
    (void)sda;
    (void)scl;
#endif

    i2c->setClock(i2cClock);

    // Probe device
    i2c->beginTransmission(address);
    return (i2c->endTransmission() == 0);
}


/**
 * Convert raw sensor reading to temperature
 *
 * - Formula from Sensirion datasheet
 * - Output range approx : -45C to +130C
 * - Raw input : 16-bit ADC value
 *
 * - returns : temperature in Celsius
 */
float STS3x_7Semi::getTemperatureC(uint16_t raw)
{
    return -45.0f + (175.0f * raw) / 65535.0f;
}


/**
 * Perform single shot temperature measurement
 *
 * - repeat : measurement precision level
 * - clockStretch : enable clock stretching mode
 * - temperature : output value in Celsius
 *
 * - returns : true if measurement successful
 */
bool STS3x_7Semi::measureSingleShot(Repeatability repeat, bool clockStretch, float &temperature)
{
    if (repeat >= REPEATABILITY_COUNT)
        return false;

    uint16_t raw;
    uint16_t cmd = SINGLE_SHOT_COMMANDS[clockStretch ? 1 : 0][repeat];

    uint8_t buffer[3];

    if (!sendCommand(cmd, buffer, 3))
        return false;

    if (getCRC(buffer, 2) != buffer[2])
        return false;

    raw = (buffer[0] << 8) | buffer[1];

    temperature = getTemperatureC(raw);

    return true;
}


/**
 * Read temperature using high precision mode
 *
 * - Uses highest repeatability
 * - Clock stretching disabled
 *
 * - returns : temperature in Celsius
 * - returns NAN if measurement fails
 */
float STS3x_7Semi::readTemperature()
{
    float temp;
    return measureSingleShot(REPEATABILITY_HIGH, false, temp) ? temp : NAN;
}


/**
 * Start periodic measurement mode
 *
 * - repeat : measurement precision level
 * - rate : measurement frequency
 *
 * Sensor will continuously measure temperature
 */
bool STS3x_7Semi::startPeriodicMeasurement(Repeatability repeat, MeasurementRates rate)
{
    if (repeat >= REPEATABILITY_COUNT)
        return false;

    if (rate >= MPS_COUNT)
        return false;

    uint16_t cmd = PERIODIC_COMMANDS[rate][repeat];

    if (!sendCommand(cmd))
        return false;

    mps = rate;

    return true;
}


/**
 * Read measurement from periodic mode
 *
 * - Blocks until measurement interval elapsed
 * - Reads latest sample from sensor buffer
 *
 * - temperature : output Celsius value
 */
bool STS3x_7Semi::blockingReadMeasurement(float &temperature)
{
    uint16_t raw;

    delay(MPS_DELAY[mps]);

    uint8_t buf[3];

    if (!sendCommand(STS3X_BLOCKING_MEASUREMENT, buf, 3, 100))
        return false;

    if (getCRC(buf, 2) != buf[2])
        return false;

    raw = (buf[0] << 8) | buf[1];

    temperature = getTemperatureC(raw);

    return true;
}


/**
 * Stop periodic measurement mode
 *
 * - Sensor returns to idle state
 */
bool STS3x_7Semi::stopMeasurement()
{
    return sendCommand(STS3X_STOP_MEASUREMENT);
}


/**
 * Read sensor status register
 *
 * - status : 16-bit sensor status flags
 *
 * - returns : true if read successful
 */
bool STS3x_7Semi::readStatusRegister(uint16_t &status)
{
    uint8_t buf[3];

    if (!sendCommand(STS3X_READ_STATUS, buf, 3))
        return false;

    if (getCRC(buf,2) != buf[2])
        return false;

    status = (buf[0] << 8) | buf[1];

    return true;
}


/**
 * Clear sensor status register
 *
 * - Resets alert and error flags
 */
bool STS3x_7Semi::clearStatusRegister()
{
    return sendCommand(STS3X_CLEAR_STATUS);
}


/**
 * Perform sensor soft reset
 *
 * - Resets internal state machine
 * - Sensor reboots internally
 */
bool STS3x_7Semi::softReset()
{
    return sendCommand(STS3X_I2C_SOFT_RESET);
}


/**
 * Send command and optionally read response
 *
 * - Handles command transmission
 * - Optional data read
 * - Timeout allows sensor conversion delay
 *
 * - cmd : sensor command
 * - data : buffer for response
 * - len : expected response length
 * - timeout : delay before read
 *
 * - returns : false if I2C transaction fails
 */
bool STS3x_7Semi::sendCommand(uint16_t cmd, uint8_t *data, size_t len, uint16_t timeout)
{
    i2c->beginTransmission(address);

    i2c->write(cmd >> 8);
    i2c->write(cmd & 0xFF);

    if (i2c->endTransmission(false) != 0)
        return false;

    if (len == 0)
        return true;

    if (timeout != 0)
        delay(timeout);

    uint8_t received = i2c->requestFrom((uint8_t)address, (uint8_t)len);

    if (received != len)
        return false;

    for (size_t i = 0; i < len; i++)
        data[i] = i2c->read();

    return true;
}


/**
 * Sensirion CRC calculation
 *
 * - Polynomial : 0x31
 * - Initial value : 0xFF
 * - Used for measurement frame validation
 *
 * - data : pointer to buffer
 * - len : number of bytes
 */
uint8_t STS3x_7Semi::getCRC(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFF;

    for (uint8_t j = 0; j < len; j++)
    {
        crc ^= data[j];

        for (uint8_t i = 0; i < 8; i++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc <<= 1;
        }
    }

    return crc;
}