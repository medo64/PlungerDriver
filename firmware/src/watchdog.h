/* Josip Medved <jmedved@jmedved.com> * www.medo64.com * MIT License */

/**
 * Handling watchdog
 * 
 * Defines:
 *   _WATCHDOG_INTERVAL <N>: Watchdog interval (1-256000 ms; values will be approximated; default 1000)
 */
// 2023-10-09: Added constants

#pragma once

#include <xc.h>

#if !defined(_16F1454) && !defined(_16F1455) && !defined(_16F1459)
#error Unsupported device
#endif

#if !defined(_WATCHDOG_INTERVAL)
#define _WATCHDOG_INTERVAL  1000
#elif _WATCHDOG_INTERVAL > 256000
#error Watchdog interval too large
#endif

/** Initializes watchdog timer. */
void watchdog_init(void);

/** Clears watchdog timer. */
#define watchdog_clear(void)  __asm("CLRWDT")
