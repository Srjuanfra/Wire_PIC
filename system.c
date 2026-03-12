#include "system.h"

void SYSTEM_Initialize(void) {
    OSCCON1 = 0x60;
    OSCFRQ = 0x06;
    
    // while(!OSCSTATbits.HFOR); // <-- comenta esto temporalmente
    
    TRISAbits.TRISA7 = 0;
    ANSELAbits.ANSA7 = 0;
    LED_A = 0;
    
    TRISAbits.TRISA6 = 0;
    ANSELAbits.ANSA6 = 0;
    LED_B = 0;
}