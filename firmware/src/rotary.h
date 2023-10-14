/* Josip Medved <jmedved@jmedved.com> * www.medo64.com * MIT License */

/**
 * Handling rotary button with quadrature encoding.
 * This library specifically interfaces with PEC12R whose encoding never
 * actually goes to 00 (01 10 11 01 10 11 ...)
 * 
 * Defines:
 *   _ROTARY_INPUT_A_PORT <X>: Input A port; e.g. A
 *   _ROTARY_INPUT_A_PIN <X>:  Input A port TRIS value; e.g. 0
 *   _ROTARY_INPUT_B_PORT <X>: Input B port; e.g. A
 *   _ROTARY_INPUT_B_PIN <X>:  Input B port TRIS value, e.g. 0
 * Required:
 *   Set pins as input (TRIS)
 */
// 2023-10-14: Setup to work with PEC12R

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

#if !defined(_ROTARY_INPUT_A_PIN)
#error Must define _ROTARY_INPUT_A_PIN
#elif (_ROTARY_INPUT_A_PIN < 0) || (_ROTARY_INPUT_A_PIN > 7)
#error _ROTARY_INPUT_A_PIN must be between 0 and 7
#endif

#if !defined(_ROTARY_INPUT_B_PIN)
#error Must define _ROTARY_INPUT_B_PIN
#elif (_ROTARY_INPUT_B_PIN < 0) || (_ROTARY_INPUT_B_PIN > 7)
#error _ROTARY_INPUT_B_PIN must be between 0 and 7
#endif


enum ROTARY_DIRECTION {
    ROTARY_DIRECTION_NONE,
    ROTARY_DIRECTION_LEFT,
    ROTARY_DIRECTION_RIGHT,
};


/* Initialize rotary control. */
void rotary_init(void);

/* Returns in which direction rotary with quadrature encoding moved. Must be called often. */
enum ROTARY_DIRECTION rotary_getDirection(void);
