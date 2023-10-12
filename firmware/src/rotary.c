#include <xc.h>
#include <stdint.h>
#include "app.h"
#include "rotary.h"

#define CONCATENATE2(x,y,z)         x ## y ## z
#define CONCATENATE(x,y,z)          CONCATENATE2(x,y,z)
#define ROTARY_INPUT_A_PORT_PIN     CONCATENATE(R, _ROTARY_INPUT_A_PORT, _ROTARY_INPUT_A_PIN)
#define ROTARY_INPUT_B_PORT_PIN     CONCATENATE(R, _ROTARY_INPUT_B_PORT, _ROTARY_INPUT_B_PIN)

uint8_t lastRotState;

inline uint8_t getRotaryState(void) {  // figure out what state we're in
    return (ROTARY_INPUT_A_PORT_PIN ? 0b10 : 0b00) | (ROTARY_INPUT_B_PORT_PIN ? 0b01 : 0b00);
}

void rotary_init(void) {
    lastRotState = getRotaryState();
}

enum ROTARY_DIRECTION rotary_getDirection(void) {
    uint8_t currRotState = getRotaryState();
    if (currRotState != lastRotState) {
        if (lastRotState == 0) {
            lastRotState = currRotState;
            if (currRotState == 1) { return ROTARY_DIRECTION_LEFT; }
            if (currRotState == 2) { return ROTARY_DIRECTION_RIGHT; }
        } else {
            lastRotState = currRotState;
        }
    }
    return ROTARY_DIRECTION_NONE;
}
