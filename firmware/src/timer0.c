#include <xc.h>
#include <stdbool.h>
#include "app.h"
#include "timer0.h"

// 24x per second (48 MHz))
#if _XTAL_FREQ == 48000000
    #define TIMER0_PRESCALER  0b010  // 1:8
    #define TIMER0_TARGET     (_XTAL_FREQ / 4 / (1 << (TIMER0_PRESCALER + 1)) / 24 / 256)  // 244
    #define TIMER0_START      ((_XTAL_FREQ - (TIMER0_TARGET * 4 * (1 << (TIMER0_PRESCALER + 1)) * 24 * 256)) / 4 / (1 << (TIMER0_PRESCALER + 1)) / 24)  // 36
#else
	#error Unknown frequency
#endif

#if TIMER0_START < 0
    #define TIMER0_START         0
#endif
#if TIMER0_TARGET > 255
    #error Cannot determine postscaler target
#endif

uint8_t counter = 0;

void timer0_init(void) {
    OPTION_REGbits.TMR0CS = 0;  // FOSC/4
#if defined(TIMER0_PRESCALER)
    OPTION_REGbits.PSA = 0;              // Prescaler is assigned to the Timer0 module
    OPTION_REGbits.PS = TIMER0_PRESCALER;
#else
    OPTION_REGbits.PSA = 1;              // Prescaler is not assigned to the Timer0 module
#endif
    TMR0 = TIMER0_START;
}

void timer0_reset(void) {
    TMR0 = TIMER0_START;
    counter = 0;
}

inline bool timer0_wasTriggered(void) {  // 24x per second
    if (INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = 0;
        if (counter == TIMER0_TARGET) {
            counter = 0;
            TMR0 = TIMER0_START;
            return true;
        }
        counter++;
    }
    return false;
}

void timer0_waitTick(void) {
    timer0_reset();
    while (!timer0_wasTriggered()) { __asm("CLRWDT"); }
}

void timer0_waitTicks(uint8_t tickCount) {
    timer0_reset();
    while (true) {
        if (tickCount == 0) { break; }
        while (!timer0_wasTriggered()) { __asm("CLRWDT"); }
        tickCount--;
    }
}
