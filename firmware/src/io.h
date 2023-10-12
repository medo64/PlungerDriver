#pragma once
#include <xc.h>

#define io_in_rot_button()               (PORTAbits.RA3 == 0)
#define io_in_rot_button_wait_release()  while (PORTAbits.RA3 == 0) { __asm("CLRWDT"); }

void io_init(void);
