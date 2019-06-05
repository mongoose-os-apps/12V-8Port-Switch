# ESP8266 based 12V switch

This is a project that uses an ESP8266 to drive 8x MOSFET power ports, rated at 2A each. It incorporates a GPIO buffer (based on [MCP23008](docs/mcp23008.pdf)) to allow the MCU to reboot without changing the port status, and a current sensor (based on [INA219](docs/ina219.pdf)) to measure total current through the device.

## Hardware

![PCB](eagle/v0-top.png)

The schematic and board layout are in the [eagle/](eagle/) directory. You can order this board from [Oshpark](https://oshpark.com/shared_projects/42dmo9fg).

Logical components in the hardware are:

*   An LDO that steps down 12V input to 3.3V, with power LED
*   An INA219 on I2C that senses current using a 0.01 Ohm shunt resistor (32A max in 80mA steps)
*   An MCP23008 8-port GPIO expander on I2C
*   8 output ports, driven by IRLML6244 mosfets (3A max per channel)
*   An ESP8266-12E which drives the current sensor and GPIO expander over I2C
    *    RST and GPIO0 are provided via pushbuttons
*   Headers:
    *    I2C breakout for additional sensor(s)
    *    4x GPIO breakouts (GPIO: 4, 5, 9, 10)
    *    Serial access to flash the ESP8266

### BOM

Bill of materials with components and sizes:

*   AM1117-3.3V
*   C1, C2: 10uF Tantalum (3216)
*   C3: 470pF ceramic (0805)
*   IC1: MCP23008 (SO18W)
*   IO0/RST: momentary switch (6mm x 3.5mm)
*   J1-J9: screw terminal (0.2" pitch)
*   J10,J11: 4-pin standard header (0.1" pitch)
*   J12-15: 3-pin standard header (0.1" pitch)
*   MDL1: ESP8266-ESP12E
*   PWR: Green LED (0805)
*   Q1-Q8: IRLML6244 (SOT23)
*   R1-R8: 1kOhm (0805)
*   R9-R10, R12-16: 10kOhm (0805)
*   R11: 330Ohm (0805)
*   R17: 0.01Ohm Shunt (2515)
*   U1: INA219 (SOT23-8)

## Software

We use drivers for [INA219](https://github.com/mongoose-os-libs/ina219-i2c/) and [MCP23008](https://github.com/mongoose-os-libs/mcp23xxx) which are readily available for Mongoose OS. For telemetry export, we use [Prometheus](https://github.com/mongoose-os-libs/prometheus-metrics) to export metrics including standard telemetry from the ESP8266 as well as custom metrics such as port state, current and voltage.

Using the Mongoose OS [RPC Subsystem](https://github.com/mongoose-os-libs/rpc-common), we expose a simple control surface to query the ports, switch them on/off or toggle them.

By default, HTTP endpoints are provided for metrics and RPCs, which can be a security hazard. For non-trusted wifi environments, we allow allow the device to close all serving ports and service RPCs via [Pubsub](https://github.com/mongoose-os-libs/mqtt) using TLS connections to a remote [Mosquitto](https://mosquitto.org/) endpoint. Metrics can be pushed via POST requests to a Prometheus [pushgateway](https://github.com/prometheus/pushgateway).

Firmware updates can be performed [over-the-air](https://github.com/mongoose-os-libs/ota-http-client), using both HTTP as well as MQTT transport.

### API
