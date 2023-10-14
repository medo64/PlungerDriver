#include <stdbool.h>
#include "appMain.h"
#include "io.h"
#include "rotary.h"
#include "ssd1306.h"
#include "watchdog.h"
#include "ticker.h"
#include "motor.h"

#define MENU_PROFILE1  1
#define MENU_PROFILE2  2
#define MENU_PROFILE3  3
#define MENU_PROFILE4  4
#define MENU_PROFILE5  5
#define MENU_FORWARD   6
#define MENU_REVERSE   7
#define MENU_SETTINGS  8

#define MENU_MAX       8

uint8_t lastMenuSelected = 0;
uint8_t currMenuSelected = 1;
uint8_t currMenuEntered = 0;

void execMain(void) {
    while(true) {
        watchdog_clear();

        // figure if rotary button moved
        switch (rotary_getDirection()) {
            case ROTARY_DIRECTION_LEFT:
                if (currMenuSelected > 1) { currMenuSelected--; }
                break;
            case ROTARY_DIRECTION_RIGHT:
                if (currMenuSelected < MENU_MAX) { currMenuSelected++; }
                break;
            default: break;
        }

        switch (currMenuSelected) {
            case MENU_PROFILE1:
            case MENU_PROFILE2:
            case MENU_PROFILE3:
            case MENU_PROFILE4:
            case MENU_PROFILE5:
                break;

            case MENU_FORWARD: {  // just move it forward
                if (io_in_internalButton() || io_in_externalButton() || io_in_rotButton()) {
                    ssd1306_displayInvert();
                    motor_setForward();
                    while (io_in_internalButton() || io_in_externalButton() || io_in_rotButton()) { watchdog_clear(); }
                    motor_setSleep();
                    ssd1306_displayNormal();
                } 
            } break;

            case MENU_REVERSE: {  // just move it in reverse
                if (io_in_internalButton() || io_in_externalButton() || io_in_rotButton()) {
                    ssd1306_displayInvert();
                    motor_setReverse();
                    while (io_in_internalButton() || io_in_externalButton() || io_in_rotButton()) { watchdog_clear(); }
                    motor_setSleep();
                    ssd1306_displayNormal();
                }
            } break;

            case MENU_SETTINGS:
                break;
        }

        // draw menu
        if (ticker_hasTicked()) {  // but not too often
            if (currMenuSelected != lastMenuSelected) {
                lastMenuSelected = currMenuSelected;
                ssd1306_displayNormal();
                ssd1306_moveTo(1, 1);
                switch (currMenuSelected) {
                    case MENU_PROFILE1: ssd1306_write16("Profile 1       "); break;
                    case MENU_PROFILE2: ssd1306_write16("Profile 2       "); break;
                    case MENU_PROFILE3: ssd1306_write16("Profile 3       "); break;
                    case MENU_PROFILE4: ssd1306_write16("Profile 4       "); break;
                    case MENU_PROFILE5: ssd1306_write16("Profile 5       "); break;
                    case MENU_FORWARD:  ssd1306_write16("Forward         "); break;
                    case MENU_REVERSE:  ssd1306_write16("Reverse         "); break;
                    case MENU_SETTINGS: ssd1306_write16("Settings        "); break;
                    default:            ssd1306_write16("                "); break;
                }
            }
        }
    }
}
