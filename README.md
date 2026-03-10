# 7Semi STS3x Arduino Library

Arduino driver for the **STS3x digital temperature sensor**.

The **STS3x** is a high-accuracy digital temperature sensor from **Sensirion** featuring an **I²C interface**, fast response time, and high measurement precision. It is designed for applications such as environmental monitoring, industrial control systems, and IoT sensing devices.

This library provides a simple interface for configuring the sensor and reading temperature values in **degrees Celsius**.

---

## Features

- High accuracy temperature measurement  
- Single-shot measurement mode  
- Periodic measurement mode  
- Configurable measurement repeatability  
- Configurable measurement rate  
- Status register reading  
- Sensor soft reset  
- CRC data validation for reliable communication  

---

## Supported Platforms

- Arduino UNO / Mega  
- ESP32  
- ESP8266  
- Any board supporting the **Wire (I²C) library**

---

## Hardware

Supported sensor:

**7Semi Digital Temperature Sensor I2C Breakout – STS3x**

---

## Connection

The **STS3x communicates using I²C**.

### I²C Connection

| STS3x Pin | MCU Pin | Description |
|-----------|--------|-------------|
| VCC | 3.3V | Sensor power |
| GND | GND | Ground |
| SCL | SCL | I²C clock |
| SDA | SDA | I²C data |

### I²C Notes

Default sensor address: 0x4A

Recommended I²C speed: 100 kHz – 400 kHz


---

## Installation

### Arduino Library Manager

1. Open **Arduino IDE**
2. Go to **Library Manager**
3. Search for **7Semi STS3x**
4. Click **Install**

---

### Manual Installation

1. Download this repository as ZIP  
2. Arduino IDE → **Sketch → Include Library → Add .ZIP Library**

---

## Example

```cpp
#include <7Semi_STS3x.h>

STS3x_7Semi sensor;

void setup()
{
  Serial.begin(115200);

  if(!sensor.begin())
  {
    Serial.println("STS3x not detected");
    while(1);
  }
}

void loop()
{
  float temperature;

  temperature = sensor.readTemperature();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  delay(1000);
}

---

## Library Overview
### Reading Temperature
```cpp
float temperature;

temperature = sensor.readTemperature();

Serial.print("Temperature: ");
Serial.println(temperature);
```
Returns the measured temperature in degrees Celsius.

## Single Shot Measurement
```cpp
float temperature;

sensor.measureSingleShot(REPEATABILITY_HIGH, false, temperature);
```

Parameters:

- Repeatability – Measurement precision level

- Clock stretching – Enable or disable clock stretching

Repeatability options:

```cpp
REPEATABILITY_LOW
REPEATABILITY_MEDIUM
REPEATABILITY_HIGH
```

## Periodic Measurement
sensor.startPeriodicMeasurement(REPEATABILITY_HIGH, MPS_2);

```cpp
float temperature;

sensor.blockingReadMeasurement(temperature);

```

Measurement rates:

```cpp
MPS_0_5
MPS_1
MPS_2
MPS_4
MPS_10
```
These correspond to measurements per second.

## Sensor Status

The sensor status register can be read for debugging and system monitoring.

```cpp
uint16_t status;

sensor.readStatusRegister(status);

Serial.print("Status: ");
Serial.println(status, HEX);

```

The status register contains flags for:

- Alert pending

- Heater status

- Temperature tracking alerts

- Reset detection

- Command errors

- Write checksum errors


## Sensor Reset

This performs a sensor soft reset and clears the internal state machine.

```cpp

sensor.softReset();

```
---

## Example Applications

### Typical applications include:

- Environmental temperature monitoring

- Industrial control systems

- IoT sensing devices

- Data logging systems

- HVAC systems

- Smart home monitoring

---

## License

### MIT License

## Author

### 7Semi

