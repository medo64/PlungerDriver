#include <xc.h>
#include "app.h"
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
    ticker_init();

    ssd1306_writeLine16(" Plunger Driver ");

    uint8_t goneLeft = false;
    bool flipped = false;
    while(true) {
        watchdog_clear();
        if (io_in_rot_button()) {
            io_in_rot_button_wait_release();
            flipped = !flipped;
            ssd1306_displayFlip(flipped);
            ssd1306_clearAll();
            ssd1306_writeLine16(" Plunger Driver ");
        }

        enum ROTARY_DIRECTION dir = rotary_getDirection();
        ssd1306_moveTo(4, 1);
        if (dir != ROTARY_DIRECTION_NONE) {
            if (dir == ROTARY_DIRECTION_LEFT) {
                goneLeft = true;
            } else {
                goneLeft = false;
            }
        }

        if (ticker_hasTicked()) {
            if (goneLeft) {
                ssd1306_writeCharacter(0x1B);
            } else {
                ssd1306_writeCharacter(0x1A);
            }            
        }
    }
}
