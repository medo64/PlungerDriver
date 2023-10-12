/* Josip Medved <jmedved@jmedved.com> * www.medo64.com * MIT License */

/**
 * Timer that "ticks" 24 times a second
 * 
 * Defines:
 *   _TICKER_USE_TIMER0: Use TMR0 for time management (default)
 */
// 2023-10-11: Refactoring

#pragma once

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "app.h"

#if !defined(_TICKER_USE_TIMER0)
    #define _TICKER_USE_TIMER0
#endif


/** Initialize timer0 */
void ticker_init(void);

/** Reset timer */
void ticker_reset(void);

/** Check if timer was triggered (24tps) since last check */
inline bool ticker_hasTicked(void);

/** waits for a full tick (1/24th of a second) */
void ticker_waitTick(void);

/** waits N ticks (1/24th of a second each) */
void ticker_waitTicks(uint8_t tickCount);
