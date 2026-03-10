/**
 * Read Sensor Status Example
 *
 * - Demonstrates periodic measurement
 * - Reads temperature continuously
 * - Prints and decodes STS3x status register
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
    Serial.println("STS3x sensor not detected");
    while (1);
  }

  /**
   * Perform sensor soft reset
   */
  Serial.println("Performing soft reset...");
  sensor.softReset();

  /**
   * Start periodic measurement
   *
   * - Repeatability : HIGH
   * - Measurement rate : 2 measurements per second
   */
  sensor.startPeriodicMeasurement(REPEATABILITY_HIGH, MPS_2);
}

void loop()
{
  float temperature;

  /**
   * Read periodic measurement
   */
  if (sensor.blockingReadMeasurement(temperature))
  {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");
  }
  else
  {
    Serial.println("Temperature read error");
  }

  /**
   * Read sensor status register
   */
  uint16_t status;

  if (sensor.readStatusRegister(status))
  {
    Serial.print("Status Register: 0x");
    Serial.println(status, HEX);

    /**
     * Bit 15
     *
     * - Alert pending
     */
    Serial.print("Alert pending: ");
    Serial.println((status & (1 << 15)) ? "YES" : "NO");

    /**
     * Bit 13
     *
     * - Heater status
     */
    Serial.print("Heater status: ");
    Serial.println((status & (1 << 13)) ? "ON" : "OFF");

    /**
     * Bit 10
     *
     * - Temperature tracking alert
     */
    Serial.print("Temperature alert: ");
    Serial.println((status & (1 << 10)) ? "ACTIVE" : "NONE");

    /**
     * Bit 4
     *
     * - System reset detected
     */
    Serial.print("Reset detected: ");
    Serial.println((status & (1 << 4)) ? "YES" : "NO");

    /**
     * Bit 1
     *
     * - Command status
     */
    Serial.print("Command status: ");
    Serial.println((status & (1 << 1)) ? "FAILED" : "OK");

    /**
     * Bit 0
     *
     * - Write data checksum status
     */
    Serial.print("Write checksum: ");
    Serial.println((status & (1 << 0)) ? "ERROR" : "OK");

    Serial.println("----------------------------");
  }
  else
  {
    Serial.println("Failed to read status register");
  }
}