/**
 * High Precision Measurement Example
 *
 * - Uses HIGH repeatability mode
 * - Demonstrates manual measurement function
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

    if (!sensor.begin(0x4B))
    {
        Serial.println("Sensor not detected");
        while (1);
    }
    delay(1000);
}

/**
 * Perform high precision temperature measurement
 */
void loop()
{
    float temperature;

    if (sensor.measureSingleShot(REPEATABILITY_HIGH, false, temperature))
    {
        Serial.print("High Precision Temperature: ");
        Serial.print(temperature);
        Serial.println(" C");
    }
    else
    {
        Serial.println("Measurement failed");
    }

    delay(2000);
}