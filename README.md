# Wire_PIC — Arduino-style I2C Library for PIC16F

A lightweight I2C master library for PIC16F microcontrollers, adapted from the Arduino `Wire` library. It keeps the same API style as Arduino, making it easy to port I2C device drivers from Arduino to PIC.

---

## Compatibility

| MCU | Compiler | IDE |
|-----|----------|-----|
| PIC16F18876 | XC8 v2.x / v3.x | MPLAB X IDE 5.50+ |

> Other PIC16F devices with MSSP module and PPS should work with minor adjustments.

---

## Features

- Arduino-style API (`Wire_begin`, `Wire_write`, `Wire_endTransmission`, etc.)
- Selectable SDA and SCL pins via PPS (Peripheral Pin Select)
- Supports the 4 native I2C logic level pins: **RC3, RC4, RB1, RB2**
- 100 kHz I2C clock (configurable via `Wire_setClock`)
- 32-byte TX/RX buffer
- Master mode only

---

## Pin Selection

The PIC16F18876 has 4 pins with native I2C logic levels. It is recommended to use these for best signal integrity:

| Pin | PPS Code |
|-----|----------|
| RC3 | `PIN_RC3` |
| RC4 | `PIN_RC4` |
| RB1 | `PIN_RB1` |
| RB2 | `PIN_RB2` |

> Other pins may work but will use standard TTL logic levels instead of I2C-specific thresholds.

---

## API Reference

### `Wire_begin(sda_pin, scl_pin)`
Initializes the I2C peripheral and assigns SDA and SCL to the selected pins.
```c
Wire_begin(PIN_RC3, PIN_RC4); // SDA = RC3, SCL = RC4
Wire_begin(PIN_RC4, PIN_RC3); // SDA = RC4, SCL = RC3
Wire_begin(PIN_RB2, PIN_RB1); // SDA = RB2, SCL = RB1
```

### `Wire_setClock(speed)`
Sets the I2C clock frequency. Default is 100000 Hz (100 kHz).
```c
Wire_setClock(100000); // 100 kHz
Wire_setClock(400000); // 400 kHz (Fast mode)
```

### `Wire_beginTransmission(address)`
Begin a transmission to the I2C device at the given address.
```c
Wire_beginTransmission(0x3C);
```

### `Wire_write(data)`
Queues a byte to be transmitted.
```c
Wire_write(0x00); // Control byte
Wire_write(0xAF); // Data byte
```

### `Wire_endTransmission()`
Transmits the queued bytes. Returns a status code.
```c
uint8_t result = Wire_endTransmission();
```

| Return value | Meaning |
|---|---|
| `WIRE_SUCCESS` (0) | Transmission successful |
| `WIRE_NACK_ON_ADDRESS` (2) | Device not found |
| `WIRE_NACK_ON_DATA` (3) | Device rejected data |
| `WIRE_OTHER_ERROR` (4) | Bus error |

### `Wire_requestFrom(address, quantity)`
Requests bytes from a slave device. Returns the number of bytes received.
```c
Wire_requestFrom(0x68, 6);
```

### `Wire_read()`
Reads a byte from the receive buffer.
```c
int byte = Wire_read();
```

### `Wire_available()`
Returns the number of bytes available to read.
```c
while(Wire_available()) {
    int b = Wire_read();
}
```

---

## Supporting Files

These files are required for the PIC to work correctly. They are not part of the Wire library itself but are necessary for any project using it.

### `config.h` — PIC Configuration Bits
Sets the hardware fuses of the microcontroller. These are programmed once and define how the PIC behaves at a low level:

| Setting | Value | Description |
|---------|-------|-------------|
| `FEXTOSC` | `OFF` | External oscillator disabled |
| `RSTOSC` | `HFINT32` | Internal oscillator at 32 MHz |
| `MCLRE` | `ON` | Master Clear pin enabled |
| `BOREN` | `ON` | Brown-out Reset enabled |
| `WDTE` | `OFF` | Watchdog Timer disabled |
| `LVP` | `ON` | Low-voltage programming (required for PICkit) |
| `PPS1WAY` | `OFF` | Allow multiple PPS reconfigurations (required for I2C pin selection) |

> `PPS1WAY = OFF` is critical — without it, the PPS registers can only be written once and pin reassignment would not work.

### `system.h` / `system.c` — System Initialization
Configures the internal oscillator to run at 32 MHz and sets up the GPIO pins used in the project.

```c
void SYSTEM_Initialize(void); // Call this first in main()

#define LED_A LATAbits.LATA7  // LED D5 on Curiosity HPC board
#define LED_B LATAbits.LATA6  // LED D4 on Curiosity HPC board
```

> Always call `SYSTEM_Initialize()` before `Wire_begin()` in your `main()`.

---



```c
#include "system.h"
#include "Wire.h"

int main(void) {
    SYSTEM_Initialize();

    // SDA = RC3, SCL = RC4
    Wire_begin(PIN_RC3, PIN_RC4);

    // Send a byte to device at address 0x3C
    Wire_beginTransmission(0x3C);
    Wire_write(0x00);
    Wire_write(0xAF);
    Wire_endTransmission();

    while(1);
    return 0;
}
```

---

## File Structure

```
Wire_PIC/
│
├── Wire.c          — I2C driver implementation
├── Wire.h          — API, pin macros and function declarations
├── README.md       — This file
├── LICENSE         — MIT License
└── examples/
    └── OLED_SSD1306/
        ├── main.c      — OLED display example
        ├── system.c    — System initialization
        ├── system.h    — LED definitions
        └── config.h    — PIC configuration bits
```

---

## Example — SSD1306 OLED

The `examples/OLED_SSD1306` folder contains a working example that displays text on a 128x64 SSD1306 OLED screen connected via I2C.

**Connections:**

| OLED | PIC16F18876 |
|------|-------------|
| SDA  | RC3         |
| SCL  | RC4         |
| VCC  | 3.3V / 5V   |
| GND  | GND         |

---

## License

MIT License — see [LICENSE](LICENSE) for details.

---

## Author

momta
