/**
 * @file main.c
 * @author momta
 * @date 2026-02-03
 * @brief Main function
 */

#include "system.h"
#include "Wire.h"
#define OLED_ADDR 0x3C

// Character definitions (5x7 pixels)
const uint8_t font5x7[][5] = {
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00},
    ['A'] = {0x7C, 0x12, 0x11, 0x12, 0x7C},
    ['D'] = {0x7F, 0x41, 0x41, 0x22, 0x1C},
    ['F'] = {0x7F, 0x09, 0x09, 0x01, 0x01},
    ['H'] = {0x7F, 0x08, 0x08, 0x08, 0x7F},
    ['I'] = {0x00, 0x41, 0x7F, 0x41, 0x00},
    ['J'] = {0x40, 0x80, 0x81, 0x7F, 0x01},
    ['N'] = {0x7F, 0x04, 0x08, 0x10, 0x7F},
    ['O'] = {0x3E, 0x41, 0x41, 0x41, 0x3E},
    ['R'] = {0x7F, 0x09, 0x19, 0x29, 0x46}
};

void OLED_Command(uint8_t cmd) {
    Wire_beginTransmission(OLED_ADDR);
    Wire_write(0x00);
    Wire_write(cmd);
    Wire_endTransmission();
}

void OLED_WriteChar(char c) {
    Wire_beginTransmission(OLED_ADDR);
    Wire_write(0x40);
    for (uint8_t i = 0; i < 5; i++) {
        Wire_write(font5x7[(uint8_t)c][i]);
    }
    Wire_write(0x00);
    Wire_endTransmission();
}

void OLED_WriteString(const char* s) {
    while (*s) {
        OLED_WriteChar(*s++);
    }
}

void OLED_SetCursor(uint8_t page, uint8_t col) {
    OLED_Command(0xB0 + page);
    OLED_Command(0x00 + (col & 0x0F));
    OLED_Command(0x10 + ((col >> 4) & 0x0F));
}

void OLED_Clear() {
    // SSD1306 has 8 pages x 128 columns = 1024 bytes
    for(uint8_t page = 0; page < 8; page++) {
        OLED_SetCursor(page, 0);
        // Send 128 zero bytes per page in chunks of 16
        for(uint8_t chunk = 0; chunk < 8; chunk++) {
            Wire_beginTransmission(OLED_ADDR);
            Wire_write(0x40); // Data mode
            for(uint8_t col = 0; col < 16; col++) {
                Wire_write(0x00);
            }
            Wire_endTransmission();
        }
    }
}

void OLED_Init() {
    __delay_ms(200);

    OLED_Command(0xAE); // Display OFF

    OLED_Command(0xD5); OLED_Command(0x80); // Clock divide
    OLED_Command(0xA8); OLED_Command(0x3F); // Multiplex ratio
    OLED_Command(0xD3); OLED_Command(0x00); // Display offset
    OLED_Command(0x40);                     // Start line 0
    OLED_Command(0x8D); OLED_Command(0x14); // Charge pump ON
    OLED_Command(0x20); OLED_Command(0x00); // Horizontal addressing mode
    OLED_Command(0x21); OLED_Command(0x00); OLED_Command(0x7F); // Column 0-127
    OLED_Command(0x22); OLED_Command(0x00); OLED_Command(0x07); // Page 0-7
    OLED_Command(0xA1);                     // Seg remap
    OLED_Command(0xC8);                     // COM scan direction
    OLED_Command(0xDA); OLED_Command(0x12); // COM pins config
    OLED_Command(0x81); OLED_Command(0xFF); // Max contrast
    OLED_Command(0xD9); OLED_Command(0xF1); // Pre-charge period
    OLED_Command(0xDB); OLED_Command(0x40); // VCOMH
    OLED_Command(0xA4);                     // Output follows RAM
    OLED_Command(0xA6);                     // Normal display

    // Clear RAM in horizontal mode (auto-increments column and page)
    Wire_beginTransmission(OLED_ADDR);
    Wire_write(0x40);
    for(uint16_t i = 0; i < 1024; i++) {
        Wire_write(0x00);
        if(i % 31 == 0 && i > 0) {
            Wire_endTransmission();
            Wire_beginTransmission(OLED_ADDR);
            Wire_write(0x40);
        }
    }
    Wire_endTransmission();

    // Switch back to page mode for cursor-based writing
    OLED_Command(0x20); OLED_Command(0x02);

    OLED_Command(0xAF); // Display ON
    __delay_ms(100);
}

int main(void) {
    SYSTEM_Initialize();
    // ------------------------------------
    // Select your I2C pins here:
    // Wire_begin( sda_pin,  scl_pin );
    //
    // Available pins with native I2C levels:
    //   PIN_RC3, PIN_RC4, PIN_RB1, PIN_RB2
    //
    // Examples:
    //   Wire_begin(PIN_RC4, PIN_RC3); // SDA=RC4, SCL=RC3
    //   Wire_begin(PIN_RB2, PIN_RB1); // SDA=RB2, SCL=RB1
    // ------------------------------------
    Wire_begin(PIN_RC3, PIN_RC4); // SDA=RC3, SCL=RC4

    OLED_Init();
    OLED_Clear();

    OLED_SetCursor(2, 20);
    OLED_WriteString("HI FAJARDO");

    while(1) {
        LED_A = 1;
        __delay_ms(500);
        LED_A = 0;
        __delay_ms(500);
    }
    return 0;
}