#include <xc.h>
#include "motor.h"


void motor_init(void) {
    TRISA5 = 0;
    TRISA4 = 0;
}

void motor_setForward(void) {
    LATA5 = 0;
    LATA4 = 1;
}

void motor_setReverse(void) {
    LATA5 = 1;
    LATA4 = 0;
}

void motor_setBrake(void) {
    LATA5 = 1;
    LATA4 = 1;
}

void motor_setSleep(void) {
    LATA5 = 0;
    LATA4 = 0;
}
