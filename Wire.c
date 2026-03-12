#include "Wire.h"
#include "system.h"

// Private library variables
static uint8_t txBuffer[BUFFER_LENGTH];
static uint8_t txBufferIndex = 0;
static uint8_t transmitAddress = 0;

static uint8_t rxBuffer[BUFFER_LENGTH];
static uint8_t rxBufferIndex = 0;
static uint8_t rxBufferLength = 0;

static void I2C_Wait(void) {
    while ((SSP1STAT & 0x04) || (SSP1CON2 & 0x1F));
}

// Configure a pin as SCL or SDA (TRIS=input, ANSEL=digital, ODCON=open-drain)
// and assign PPS input/output registers
// pps_in_val  = value to load into SSP1CLKPPS or SSP1DATPPS
// pps_out_val = 0x14 for SCL1, 0x15 for SDA1
static void I2C_ConfigPin(uint8_t pin, uint8_t pps_in_val, uint8_t pps_out_val) {
    switch(pin) {
        case PIN_RB1:
            SSP1CLKPPS = (pps_out_val == 0x14) ? 0x09 : SSP1CLKPPS;
            SSP1DATPPS = (pps_out_val == 0x15) ? 0x09 : SSP1DATPPS;
            RB1PPS = pps_out_val;
            TRISBbits.TRISB1 = 1; ANSELBbits.ANSB1 = 0; ODCONBbits.ODCB1 = 1;
            break;
        case PIN_RB2:
            SSP1CLKPPS = (pps_out_val == 0x14) ? 0x0A : SSP1CLKPPS;
            SSP1DATPPS = (pps_out_val == 0x15) ? 0x0A : SSP1DATPPS;
            RB2PPS = pps_out_val;
            TRISBbits.TRISB2 = 1; ANSELBbits.ANSB2 = 0; ODCONBbits.ODCB2 = 1;
            break;
        case PIN_RC3:
            SSP1CLKPPS = (pps_out_val == 0x14) ? 0x13 : SSP1CLKPPS;
            SSP1DATPPS = (pps_out_val == 0x15) ? 0x13 : SSP1DATPPS;
            RC3PPS = pps_out_val;
            TRISCbits.TRISC3 = 1; ANSELCbits.ANSC3 = 0; ODCONCbits.ODCC3 = 1;
            break;
        case PIN_RC4:
            SSP1CLKPPS = (pps_out_val == 0x14) ? 0x14 : SSP1CLKPPS;
            SSP1DATPPS = (pps_out_val == 0x15) ? 0x14 : SSP1DATPPS;
            RC4PPS = pps_out_val;
            TRISCbits.TRISC4 = 1; ANSELCbits.ANSC4 = 0; ODCONCbits.ODCC4 = 1;
            break;
        default:
            break;
    }
}

void Wire_begin(uint8_t sda_pin, uint8_t scl_pin) {
    I2C_ConfigPin(scl_pin, 0x00, 0x14); // SCL output = 0x14
    I2C_ConfigPin(sda_pin, 0x00, 0x15); // SDA output = 0x15

    SSP1CON1 = 0x28; // Master mode, SSPEN=1
    SSP1STAT = 0x00;
    Wire_setClock(100000);
}

void Wire_setClock(uint32_t speed) {
    SSP1ADD = (uint8_t)((_XTAL_FREQ / (4 * speed)) - 1);
}

void Wire_beginTransmission(uint8_t address) {
    transmitAddress = address;
    txBufferIndex = 0;
}

size_t Wire_write(uint8_t data) {
    if (txBufferIndex < BUFFER_LENGTH) {
        txBuffer[txBufferIndex++] = data;
        return 1;
    }
    return 0;
}

uint8_t Wire_endTransmission(void) {
    I2C_Wait();
    SSP1CON2bits.SEN = 1; // START
    while(SSP1CON2bits.SEN);

    // Send Address + Write (0)
    SSP1BUF = (uint8_t)(transmitAddress << 1);
    while(SSP1STATbits.BF);
    if(SSP1CON2bits.ACKSTAT) return WIRE_NACK_ON_ADDRESS;

    // Send buffer data
    for(uint8_t i = 0; i < txBufferIndex; i++) {
        I2C_Wait();
        SSP1BUF = txBuffer[i];
        while(SSP1STATbits.BF);
        if(SSP1CON2bits.ACKSTAT) return WIRE_NACK_ON_DATA;
    }

    I2C_Wait();
    SSP1CON2bits.PEN = 1; // STOP
    while(SSP1CON2bits.PEN);

    return WIRE_SUCCESS;
}

uint8_t Wire_requestFrom(uint8_t address, uint8_t quantity) {
    if(quantity > BUFFER_LENGTH) quantity = BUFFER_LENGTH;

    I2C_Wait();
    SSP1CON2bits.SEN = 1; // START
    while(SSP1CON2bits.SEN);

    // Send Address + Read (1)
    SSP1BUF = (uint8_t)((address << 1) | 1);
    while(SSP1STATbits.BF);

    rxBufferIndex = 0;
    rxBufferLength = quantity;

    for(uint8_t i = 0; i < quantity; i++) {
        I2C_Wait();
        SSP1CON2bits.RCEN = 1;
        while(!SSP1STATbits.BF);
        rxBuffer[i] = SSP1BUF;

        I2C_Wait();
        if(i < (quantity - 1)) SSP1CON2bits.ACKDT = 0; // ACK
        else SSP1CON2bits.ACKDT = 1;                    // NACK at end
        SSP1CON2bits.ACKEN = 1;
        while(SSP1CON2bits.ACKEN);
    }

    I2C_Wait();
    SSP1CON2bits.PEN = 1; // STOP
    while(SSP1CON2bits.PEN);

    return quantity;
}

int Wire_read(void) {
    if(rxBufferIndex < rxBufferLength) {
        return rxBuffer[rxBufferIndex++];
    }
    return -1;
}

int Wire_available(void) {
    return rxBufferLength - rxBufferIndex;
}

int Wire_peek(void)                          { return (rxBufferIndex < rxBufferLength) ? rxBuffer[rxBufferIndex] : -1; }
void Wire_flush(void)                        { }
void Wire_setWireTimeout(uint32_t t, bool r) { }
bool Wire_getWireTimeoutFlag(void)           { return false; }