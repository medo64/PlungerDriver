#pragma once
#include <xc.h>

#define io_in_rot_button()               (PORTAbits.RA3 == 0)
#define io_in_rot_button_wait_release()  while (PORTAbits.RA3 == 0) { __asm("CLRWDT"); }

#define io_in_rotA()              (PORTCbits.RC4 == 0)
#define io_in_rotB()              (PORTCbits.RC5 == 0)


void io_init(void);
