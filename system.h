#ifndef SYSTEM_H
#define SYSTEM_H

#include <xc.h>
#include "config.h" 

#define _XTAL_FREQ 32000000 

void SYSTEM_Initialize(void);
#define LED_A LATAbits.LATA7
#define LED_B LATAbits.LATA6
#endif