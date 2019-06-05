# ESP8266 based 12V switch

This is a project that uses an ESP8266 to drive 8x MOSFET power ports,
rated at 2A each. It incorporates a GPIO buffer (based on MCP23008) to
allow the MCU to reboot without changing the port status, and a current
sensor (based on INA219) to measure total current through the device.

## Hardware

![PCB](eagle/v0-top.png){:height="200px" width="300px"}

The schematic and board layout are in the [eagle/](eagle/) directory.
Logical components in the hardware are:

*   An LDO that steps down 12V input to 3.3V
*   An INA219 on I2C that senses current using a 0.01 Ohm shunt resistor (32A max in 80mA steps)
*   An MCP23008 8-port GPIO expander on I2C
*   8 output ports, driven by IRLML6244 mosfets (3A max per channel)
*   An ESP8266-12E which drives the current sensor and GPIO expander over I2C
*   Headers:
    *    I2C breakout for additional sensor(s)
    *    4x GPIO breakouts (GPIO: 4, 5, 9, 10)
    *    Serial access to flash the ESP8266

### BOM

## Software

### API
