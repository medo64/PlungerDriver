#include <xc.h>
#include "app.h"
#include "rotary.h"
#include "ssd1306.h"
#include "watchdog.h"
#include "timer0.h"

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
    timer0_init();

    ssd1306_writeLine16(" Plunger Driver ");

    uint8_t goneLeft = false;
    while(true) {
        watchdog_clear();
        enum ROTARY_DIRECTION dir = rotary_getDirection();
        ssd1306_moveTo(4, 1);
        if (dir != ROTARY_DIRECTION_NONE) {
            if (dir == ROTARY_DIRECTION_LEFT) {
                goneLeft = true;
            } else {
                goneLeft = false;
            }
        }
        
        if (timer0_wasTriggered()) {
            if (goneLeft) {
                ssd1306_write("L");
            } else {
                ssd1306_write("R");
            }            
        }
    }
}