#include <xc.h>
#include "app.h"
#include "appMain.h"
#include "io.h"
#include "rotary.h"
#include "ssd1306.h"
#include "watchdog.h"
#include "ticker.h"

void init(void) {
    interruptsDisable();

    // Oscillator
    OSCCONbits.IRCF = 0b1111;  // 16 MHz or 48 MHz HF
    OSCCONbits.SPLLMULT = 1;   // 3x PLL is enabled
    OSCCONbits.SPLLEN = 1;     // PLL is enabled
    ACTCONbits.ACTSRC = 1;     // The HFINTOSC oscillator is tuned using Fll-speed USB events
    ACTCONbits.ACTEN = 1;      // ACT is enabled, updates to OSCTUNE are exclusive to the ACT
}

void main(void) {
    init();
    watchdog_init();

    i2c_master_init();
    ssd1306_init();
    rotary_init();
    ticker_init();

    ssd1306_writeLine16(" Plunger Driver ");

    execMain();
}
