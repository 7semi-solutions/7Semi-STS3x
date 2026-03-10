/**
 * Periodic Measurement Mode Example
 *
 * - Starts periodic measurement
 * - Reads temperature continuously
 *
 * Measurement Rate:
 * - 2 measurements per second
 *
 * Hardware:
 * - STS3x temperature sensor
 * - Arduino / ESP32 
 *
 * Connection:
 *
 *  STS3x      MCU
 *  ----------------
 *  VCC   ->   3.3V
 *  GND   ->   GND
 *  SDA   ->   SDA
 *  SCL   ->   SCL
 *
 * Default I2C Address:
 * - 0x4A
 */

#include <7Semi_STS3x.h>

STS3x_7Semi sensor;

void setup()
{
    Serial.begin(115200);

    if (!sensor.begin())
    {
        Serial.println("Sensor not found");
        while (1);
    }

    /**
     * Start periodic measurement
     *
     * - Repeatability: HIGH
     * - Rate: 2 measurements per second
     */
    sensor.startPeriodicMeasurement(REPEATABILITY_HIGH, MPS_2);
}

void loop()
{
    float temperature;

    if (sensor.blockingReadMeasurement(temperature))
    {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" C");
    }
    else
    {
        Serial.println("Read error");
    }
}