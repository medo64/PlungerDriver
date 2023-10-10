/* Josip Medved <jmedved@jmedved.com> * www.medo64.com * MIT License */

/**
 * Timer that "ticks" 24 times a second
 * 
 * Defines:
 *   _ROTARY_INPUT_A_PORT <N>: Input A port; e.g. PORTAbits.RA0 
 *   _ROTARY_INPUT_B_PORT <N>: Input B port, e.g. PORTAbits.RA1
 *   _ROTARY_FILTER_DETENT:    Filter input to every 4th value (single detent)
 */
// 2023-10-09: Included port definition

#pragma once

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>


/** Initialize timer0 */
void timer0_init(void);

/** Reset timer */
void timer0_reset(void);

/** Check if timer was triggered (24tps) since last check */
inline bool timer0_wasTriggered(void);

/** waits for a full tick (1/24th of a second) */
void timer0_waitTick(void);

/** waits N ticks (1/24th of a second each) */
void timer0_waitTicks(uint8_t tickCount);
