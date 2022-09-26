# Remote Procedure Call Proof Of Concept (json_rpc)

This app demonstrates a simple JSON based remote procedure call architecture.

## json_rpc

This app runs on a SAM E54 XPRO dev board with an I/O1 XPRO expansion card.
Actually, to be useful, it runs on a _pair_ of such systems.

The basic operation:
* Every second, it reads the value of the light sensor on the I/O1 XPRO board.
* It transmits the light level to the "other" system via a JSON RPC message.
* When the User Button changes, it sends an RPC message to the other system.

At the same time, the system listens for incoming JSON messages:
* If it receives a "light update" message, it adjusts the brightness of the
IO1's LED via PWM.
* If it receives a "button change" message, it sets the on-board LED to
track the button state.

Note: Actually, both systems run the same code, listening for changes to
the button state and monitoring the light level.

### Program structure

`json_rpc` is implemented as three FreeRTOS tasks:
* `listen_thread` waits for a newline terminated JSON message to be received
on the serial input.  It then parses the string and calls the appropriate local
function.
* `sensor_thread` runs once a second.  It uses the ADC to read the light level
on the I/O1 light sensor, packages that up as a JSON string, and sends it to the
other system over the serial port.
* `button_thread` runs whenever the button changes state.  It reads the state of
the button, packages that up as a JSON string, and sends it to the other system
over the serial port.

The JSON RPC messages have the following form:
```
{"fn":"light_sensor_state", "args":[0.342]}\r\n

{"fn":"button_state", "args":[true]}\r\n

```

### Not yet implemented

This demo app shows the basic runtime structure for generating and receiving
remote procedure calls.  

It does not implement the compile-time code that parses message descriptors and
emits the C runtime code that translates C calls into JSON RPC strings and the
corresponding code that parses JSON RPC strings and calls local C function.

### Hardware / Driver notes

* [ATSAME54P20A Product Page](https://www.microchip.com/en-us/product/ATSAME54P20A)
* [SAM E54 Xplained Pro User's Guide](https://ww1.microchip.com/downloads/en/DeviceDoc/70005321A.pdf)
* [SAM D5X/E5X Family Datasheet](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU32/ProductDocuments/DataSheets/SAM_D5x_E5x_Family_Data_Sheet_DS60001507G.pdf)

[I/O1 Xplained Pro](https://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en589640)

| I/O1 | EXT1.n | SAM E54 XPRO | Function |
| --- | ------ | ------- | -------- |
| PWM LED | EXT1.7 | PB08 / TC4/WO[0] | variable LED |
| Light Sensor | EXT1.3 | PB04 / ADC1/AIN[6] | Light Sensor |
| UART_RX loopback | EXT1.13 | PA05 / SERCOM0.1 | Serial RX candidate |
| UART_TX loopback | EXT1.14 | PA04 / SERCOM0.0 | Serial TX candidate |
|              |  -na-  | PC18 | On-board LED (low true) |
|              |  -na-  | PB31 | On-board button (low true, need pullup) |
|              |  -na-  | PB24 / SERCOM2.1 | VCOM RXD |
|              |  -na-  | PB25 / SERCOM2.0 | VCOM TXD |
