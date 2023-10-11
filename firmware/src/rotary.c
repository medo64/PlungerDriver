#include <stdint.h>
#include "rotary.h"

#define CONCATENATE2(x,y,z)         x ## y ## z
#define CONCATENATE(x,y,z)          CONCATENATE2(x,y,z)
#define ROTARY_INPUT_A_PORT_PIN     CONCATENATE(R, _ROTARY_INPUT_A_PORT, _ROTARY_INPUT_A_PIN)
#define ROTARY_INPUT_B_PORT_PIN     CONCATENATE(R, _ROTARY_INPUT_B_PORT, _ROTARY_INPUT_B_PIN)
#define ROTARY_INPUT_A_TRIS_PIN     CONCATENATE(TRIS, _ROTARY_INPUT_A_PORT, _ROTARY_INPUT_A_PIN)
#define ROTARY_INPUT_B_TRIS_PIN     CONCATENATE(TRIS, _ROTARY_INPUT_B_PORT, _ROTARY_INPUT_B_PIN)

uint8_t lastRotState = 0;
uint8_t lastRotIndentState = 128;  // to filter movement in between indents a bit

uint8_t getRotaryState() {  // figure out what state we're in
    if (!ROTARY_INPUT_A_PORT_PIN && !ROTARY_INPUT_B_PORT_PIN) { return 0; }
    if ( ROTARY_INPUT_A_PORT_PIN && !ROTARY_INPUT_B_PORT_PIN) { return 1; }
    if ( ROTARY_INPUT_A_PORT_PIN &&  ROTARY_INPUT_B_PORT_PIN) { return 2; }
    return 3;
}

void rotary_init() {
    ROTARY_INPUT_A_TRIS_PIN = 1;
    ROTARY_INPUT_B_TRIS_PIN = 1;

    lastRotState = getRotaryState();
    lastRotIndentState = 128;  // to filter movement in between indents a bit
}


enum ROTARY_DIRECTION rotary_getDirection() {
    uint8_t currRotState = getRotaryState();
    if (currRotState != lastRotState) {
        uint8_t diff = (currRotState - lastRotState) & 0b11;
        lastRotState = currRotState;
        if (diff == 1) { lastRotIndentState++; }
        if (diff == 3) { lastRotIndentState--; }
#if defined(_ROTARY_12_PPR)
        if (currRotState == 0) {  // increment every 4 cycles
#else
        if ((currRotState == 0) || (currRotState == 2)) {  // increment every 2 cycles
#endif
            if (lastRotIndentState > 128) { lastRotIndentState = 128; return ROTARY_DIRECTION_RIGHT; }
            if (lastRotIndentState < 128) { lastRotIndentState = 128; return ROTARY_DIRECTION_LEFT; }
        }
    }
    return ROTARY_DIRECTION_NONE;
}
