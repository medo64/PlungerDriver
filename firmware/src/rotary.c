#include <stdint.h>
#include "rotary.h"

uint8_t lastRotState = 0;

#if defined(_ROTARY_FILTER_DETENT_12) || defined(_ROTARY_FILTER_DETENT_24)
uint8_t lastRotIndentState = 128;  // to filter movement in between indents a bit
#endif

uint8_t getRotaryState() {  // figure out what state we're in
    if (!_ROTARY_INPUT_A_PORT && !_ROTARY_INPUT_B_PORT) { return 0; }
    if ( _ROTARY_INPUT_A_PORT && !_ROTARY_INPUT_B_PORT) { return 1; }
    if ( _ROTARY_INPUT_A_PORT &&  _ROTARY_INPUT_B_PORT) { return 2; }
    return 3;
}

void rotary_init() {
#if defined(_ROTARY_INPUT_A_TRIS)
    _ROTARY_INPUT_A_TRIS = 1;
#endif
#if defined(_ROTARY_INPUT_B_TRIS)
    _ROTARY_INPUT_B_TRIS = 1;
#endif

    lastRotState = getRotaryState();
#if defined(_ROTARY_FILTER_DETENT_12) || defined(_ROTARY_FILTER_DETENT_24)
    lastRotIndentState = 128;  // to filter movement in between indents a bit
#endif
}


enum ROTARY_DIRECTION rotary_getDirection() {
    enum ROTARY_DIRECTION newMove = ROTARY_DIRECTION_NONE;
    uint8_t currRotState = getRotaryState();
    if (currRotState != lastRotState) {
        uint8_t diff = (currRotState - lastRotState) & 0b11;
        if (diff == 1) { lastRotIndentState++; }
        if (diff == 3) { lastRotIndentState--; }
#if defined(_ROTARY_FILTER_DETENT_12)
        if (currRotState == 0) {  // just increment on indents
#else
        if ((currRotState == 0) || (currRotState == 2)) {  // just increment on indents
#endif
            if (lastRotIndentState > 128) { newMove = ROTARY_DIRECTION_RIGHT; }
            if (lastRotIndentState < 128) { newMove = ROTARY_DIRECTION_LEFT; }
            lastRotIndentState = 128;
        }
        lastRotState = currRotState;
    }
    return newMove;
}
