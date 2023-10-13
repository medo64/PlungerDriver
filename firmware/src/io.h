#pragma once
#include <xc.h>

#define io_in_rotA()              (PORTCbits.RC4 == 0)
#define io_in_rotB()              (PORTCbits.RC5 == 0)
#define io_in_rotButton()         (PORTAbits.RA3 == 0)

#define io_in_internalButton()    (PORTAbits.RA0 == 0)
#define io_in_externalButton()    (PORTAbits.RA1 == 0)


void io_init(void);
