#pragma once

#define interruptsEnable()   GIE = 1
#define interruptsDisable()  GIE = 0

#define systemReset()   asm("RESET");

void init(void);


// CONFIG1
#pragma config FOSC     = INTOSC    // INTOSC oscillator: I/O function on CLKIN pin
#pragma config WDTE     = ON        // WDT enabled
#pragma config PWRTE    = ON        // PWRT enabled
#pragma config MCLRE    = OFF       // MCLR/VPP pin function is digital input
#pragma config CP       = ON        // Program memory code protection is enabled
#pragma config BOREN    = ON        // Brown-out Reset enabled
#pragma config CLKOUTEN = OFF       // CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin
#pragma config IESO     = OFF       // Internal/External Switchover Mode is disabled
#pragma config FCMEN    = OFF       // Fail-Safe Clock Monitor is disabled

// CONFIG2
#pragma config WRT      = OFF       // Write protection off
#pragma config CPUDIV   = NOCLKDIV  // NO CPU system divide
#pragma config USBLSCLK = 48MHz     // System clock expects 48 MHz, FS/LS USB CLKENs divide-by is set to 8
#pragma config PLLMULT  = 3x        // 3x Output Frequency Selected
#pragma config PLLEN    = ENABLED   // 3x or 4x PLL Enabled
#pragma config STVREN   = ON        // Stack Overflow or Underflow will cause a Reset
#pragma config BORV     = LO        // Brown-out Reset Voltage (Vbor), low trip point selected.
#pragma config LPBOR    = ON        // Low-Power BOR is enabled
#pragma config LVP      = OFF       // High-voltage on MCLR/VPP must be used for programming
