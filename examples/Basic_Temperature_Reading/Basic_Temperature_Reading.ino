/**
 * Basic Temperature Reading Example
 *
 * - Demonstrates how to read temperature from the STS3x sensor
 * - Uses single-shot measurement mode
 * - Prints temperature in °C
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

// Arduino
#define SDA -1
#define SCL -1

// // ESP32
// #define SDA 21
// #define SCL 22

#define CLOCK_SPEED 100000

STS3x_7Semi sensor;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("7Semi STS3x Example");
    Serial.println("--------------------");

    /**
     * Initialize sensor
     *
     * - address : I2C sensor address
     * - port : Wire interface
     * - clock : I2C clock speed
     */
    if (!sensor.begin(STS3X_I2C_ADDR, Wire, CLOCK_SPEED))
    {
        Serial.println("STS3x sensor not detected!");
        while (1);
    }

    Serial.println("STS3x initialized successfully");
}

void loop()
{
    /**
     * Read temperature
     *
     * - Uses high repeatability measurement
     * - Returns NAN if read fails
     */
    float temperature = sensor.readTemperature();

    if (!isnan(temperature))
    {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" °C");
    }
    else
    {
        Serial.println("Temperature read failed");
    }

    delay(1000);
}