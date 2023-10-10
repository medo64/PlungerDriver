/* Josip Medved <jmedved@jmedved.com> * www.medo64.com * MIT License */

/**
 * Handling rotary button with quadrature encoding
 * 
 * Defines:
 *   _ROTARY_INPUT_A_PORT <N>: Input A port; e.g. PORTAbits.RA0
 *   _ROTARY_INPUT_B_PORT <N>: Input B port, e.g. PORTAbits.RA1
 *   _ROTARY_INPUT_A_TRIS <N>: Input A port TRIS value; e.g. TRISAbits.TRISA0; optional
 *   _ROTARY_INPUT_B_TRIS <N>: Input B port TRIS value, e.g. TRISAbits.TRISA1; optional
 *   _ROTARY_FILTER_DETENT_12: Filter input to every 4th value (12 PPR)
 *   _ROTARY_FILTER_DETENT_24: Filter input to every 2nd value (24 PPR)
 */
// 2023-10-09: Included port definition

#pragma once

#include <xc.h>
#include <stdbool.h>
#include "app.h"

#if !defined(_ROTARY_INPUT_A_PORT)
#error Must define _ROTARY_INPUT_A_PORT
#endif

#if !defined(_ROTARY_INPUT_B_PORT)
#error Must define _ROTARY_INPUT_B_PORT
#endif

#if !defined(_ROTARY_FILTER_DETENT_12) && !defined(_ROTARY_FILTER_DETENT_24)
#error Must define either _ROTARY_FILTER_DETENT_12 or _ROTARY_FILTER_DETENT_24
#elif defined(_ROTARY_FILTER_DETENT_12) && defined(_ROTARY_FILTER_DETENT_24)
#error Cannot define both _ROTARY_FILTER_DETENT_12 and _ROTARY_FILTER_DETENT_24
#endif


enum ROTARY_DIRECTION {
    ROTARY_DIRECTION_NONE,
    ROTARY_DIRECTION_LEFT,
    ROTARY_DIRECTION_RIGHT,
};


/* Initialize rotary control. */
void rotary_init();

/* Returns in which direction rotary with quadrature encoding moved. Must be called often. */
enum ROTARY_DIRECTION rotary_getDirection();
