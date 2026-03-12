#ifndef CONFIG_H
#define CONFIG_H

#include <xc.h>

// CONFIG1: Clock Configuration
#pragma config FEXTOSC = OFF    // External oscillator disabled
#pragma config RSTOSC = HFINT32 // Internal oscillator at 32 MHz (HFINTOSC)
#pragma config CLKOUTEN = OFF   // Disable clock output on pin
#pragma config CSWEN = ON       // Allow clock switching via software
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor enabled

// CONFIG2: Reset and Monitoring
#pragma config MCLRE = ON       // Master Clear pin enabled
#pragma config PWRTE = OFF      // Power-up Timer disabled
#pragma config LPBOREN = OFF    // Low-Power Brown-out Reset disabled
#pragma config BOREN = ON       // Brown-out Reset enabled
#pragma config BORV = LO        // Brown-out Reset voltage set to low (1.9V)
#pragma config ZCD = OFF        // Zero-cross detect disabled
#pragma config PPS1WAY = OFF    // Allow multiple PPS changes (Crucial for I2C)
#pragma config STVREN = ON      // Stack overflow/underflow reset enabled

// CONFIG3: Watchdog Timer (WDT)
#pragma config WDTE = OFF       // Watchdog Timer disabled to prevent unexpected resets

// CONFIG4: Memory and Programming
#pragma config WRT = OFF        // Flash memory write protection disabled
#pragma config LVP = ON         // Low-voltage programming enabled (Required for PICkit)

// CONFIG5: Code Protection
#pragma config CP = OFF         // Program memory code protection disabled
#pragma config CPD = OFF        // Data EEPROM code protection disabled

#define _XTAL_FREQ 32000000     // Inform the compiler that the clock runs at 32 MHz

#endif
