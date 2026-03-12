#ifndef WIRE_H
#define WIRE_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define BUFFER_LENGTH 32

// Return states
#define WIRE_SUCCESS           0
#define WIRE_DATA_TOO_LONG     1
#define WIRE_NACK_ON_ADDRESS   2
#define WIRE_NACK_ON_DATA      3
#define WIRE_OTHER_ERROR       4

// ---------------------------------------------------------------------------
// Pin definitions for SDA and SCL selection
// Only these 4 pins have native I2C logic levels on the PIC16F18876
// Use them in Wire_begin() like:
//   Wire_begin(PIN_RC4, PIN_RC3); // SDA = RC4, SCL = RC3
// ---------------------------------------------------------------------------
#define PIN_RB1  0x01
#define PIN_RB2  0x02
#define PIN_RC3  0x13
#define PIN_RC4  0x14
// ---------------------------------------------------------------------------

// Wire_begin(sda_pin, scl_pin)
// Example: Wire_begin(PIN_RC4, PIN_RC3) -> SDA=RC4, SCL=RC3
void Wire_begin(uint8_t sda_pin, uint8_t scl_pin);
void Wire_setClock(uint32_t speed);
void Wire_beginTransmission(uint8_t address);
size_t Wire_write(uint8_t data);
uint8_t Wire_endTransmission(void);
uint8_t Wire_requestFrom(uint8_t address, uint8_t quantity);
int Wire_read(void);
int Wire_available(void);
int Wire_peek(void);
void Wire_flush(void);

void Wire_onReceive(void (*function)(int));
void Wire_onRequest(void (*function)(void));

void Wire_setWireTimeout(uint32_t timeout, bool reset_on_timeout);
bool Wire_getWireTimeoutFlag(void);

#endif