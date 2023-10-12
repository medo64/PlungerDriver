/* Josip Medved <jmedved@jmedved.com> * www.medo64.com * MIT License */

#include <xc.h>
#include "app.h"
#include "watchdog.h"

void watchdog_init(void) {
#if _WATCHDOG_INTERVAL <= 1
    WDTCONbits.WDTPS = 0b00000;  // 1 ms nominal
#elif _WATCHDOG_INTERVAL <= 2
    WDTCONbits.WDTPS = 0b00001;  // 2 ms nominal
#elif _WATCHDOG_INTERVAL <= 4
    WDTCONbits.WDTPS = 0b00010;  // 4 ms nominal
#elif _WATCHDOG_INTERVAL <= 8
    WDTCONbits.WDTPS = 0b00011;  // 8 ms nominal
#elif _WATCHDOG_INTERVAL <= 16
    WDTCONbits.WDTPS = 0b00100;  // 16 ms nominal
#elif _WATCHDOG_INTERVAL <= 32
    WDTCONbits.WDTPS = 0b00101;  // 32 ms nominal
#elif _WATCHDOG_INTERVAL <= 64
    WDTCONbits.WDTPS = 0b00110;  // 64 ms nominal
#elif _WATCHDOG_INTERVAL <= 128
    WDTCONbits.WDTPS = 0b00111;  // 128 ms nominal
#elif _WATCHDOG_INTERVAL <= 256
    WDTCONbits.WDTPS = 0b01000;  // 256 ms nominal
#elif _WATCHDOG_INTERVAL <= 512
    WDTCONbits.WDTPS = 0b01001;  // 512 ms nominal
#elif _WATCHDOG_INTERVAL <= 1000
    WDTCONbits.WDTPS = 0b01010;  // 1 s nominal
#elif _WATCHDOG_INTERVAL <= 2000
    WDTCONbits.WDTPS = 0b01011;  // 2 s nominal
#elif _WATCHDOG_INTERVAL <= 4000
    WDTCONbits.WDTPS = 0b01100;  // 4 s nominal
#elif _WATCHDOG_INTERVAL <= 8000
    WDTCONbits.WDTPS = 0b01101;  // 8 s nominal
#elif _WATCHDOG_INTERVAL <= 16000
    WDTCONbits.WDTPS = 0b01110;  // 16 s nominal
#elif _WATCHDOG_INTERVAL <= 32000
    WDTCONbits.WDTPS = 0b01111;  // 32 s nominal
#elif _WATCHDOG_INTERVAL <= 64000
    WDTCONbits.WDTPS = 0b10000;  // 64 s nominal
#elif _WATCHDOG_INTERVAL <= 128000
    WDTCONbits.WDTPS = 0b10001;  // 128 s nominal
#else
    WDTCONbits.WDTPS = 0b10010;  // 256 s nominal
#endif
    watchdog_clear();
}
