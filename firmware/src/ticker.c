#include <xc.h>
#include <stdbool.h>
#include "app.h"
#include "ticker.h"

// 24x per second (48 MHz))
#if _XTAL_FREQ == 48000000
    #define TICKER_TIMER0_PRESCALER  0b010  // 1:8
    #define TICKER_TIMER0_TARGET     (_XTAL_FREQ / 4 / (1 << (TICKER_TIMER0_PRESCALER + 1)) / 24 / 256)  // 244
    #define TICKER_TIMER0_START      ((_XTAL_FREQ - (TICKER_TIMER0_TARGET * 4 * (1 << (TICKER_TIMER0_PRESCALER + 1)) * 24 * 256)) / 4 / (1 << (TICKER_TIMER0_PRESCALER + 1)) / 24)  // 36
#else
	#error Unknown frequency
#endif

#if TICKER_TIMER0_START < 0
    #define TICKER_TIMER0_START         0
#endif
#if TICKER_TIMER0_TARGET > 255
    #error Cannot determine postscaler target
#endif

uint8_t counter = 0;

void ticker_init(void) {
    OPTION_REGbits.TMR0CS = 0;  // FOSC/4
#if defined(TICKER_TIMER0_PRESCALER)
    OPTION_REGbits.PSA = 0;              // Prescaler is assigned to the Timer0 module
    OPTION_REGbits.PS = TICKER_TIMER0_PRESCALER;
#else
    OPTION_REGbits.PSA = 1;              // Prescaler is not assigned to the Timer0 module
#endif
    TMR0 = TICKER_TIMER0_START;
}

void ticker_reset(void) {
    TMR0 = TICKER_TIMER0_START;
    counter = 0;
}

inline bool ticker_hasTicked(void) {  // 24x per second
    if (INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = 0;
        if (counter == TICKER_TIMER0_TARGET) {
            counter = 0;
            TMR0 = TICKER_TIMER0_START;
            return true;
        }
        counter++;
    }
    return false;
}

void ticker_waitTick(void) {
    ticker_reset();
    while (!ticker_hasTicked()) { __asm("CLRWDT"); }
}

void ticker_waitTicks(uint8_t tickCount) {
    ticker_reset();
    while (true) {
        if (tickCount == 0) { break; }
        while (!ticker_hasTicked()) { __asm("CLRWDT"); }
        tickCount--;
    }
}
