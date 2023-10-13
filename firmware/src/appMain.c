#include <stdbool.h>
#include "appMain.h"
#include "io.h"
#include "rotary.h"
#include "ssd1306.h"
#include "watchdog.h"
#include "ticker.h"

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

enum ROTARY_DIRECTION pendingRotaryDirection = ROTARY_DIRECTION_NONE;

bool pendingRotaryButton = false;
bool pendingInternalButton = false;
bool pendingExternalButton = false;

bool lastRotaryButton = false;
bool lastInternalButton = false;
bool lastExternalButton = false;

void execMain(void) {
    while(true) {
        watchdog_clear();

        // figure if internal button was pressed (and ignore it until release))
        bool currInternalButton = io_in_internalButton();
        if (currInternalButton != lastInternalButton) {
            lastInternalButton = currInternalButton;
            if (currInternalButton) { pendingInternalButton = true; }
        }

        // figure if external button was pressed (and ignore it until release))
        bool currExternalButton = io_in_externalButton();
        if (currExternalButton != lastExternalButton) {
            lastExternalButton = currExternalButton;
            if (currExternalButton) { pendingExternalButton = true; }
        }

        // figure if rotary button was pressed (and ignore it until release))
        bool currRotaryButton = io_in_rotButton();
        if (currRotaryButton != lastRotaryButton) {
            lastRotaryButton = currRotaryButton;
            if (currRotaryButton) { pendingRotaryButton = true; }
        }

        // figure if rotary button moved
        switch (rotary_getDirection()) {
            case ROTARY_DIRECTION_LEFT: pendingRotaryDirection = ROTARY_DIRECTION_LEFT; break;
            case ROTARY_DIRECTION_RIGHT: pendingRotaryDirection = ROTARY_DIRECTION_RIGHT; break;
            default: break;
        }

        // handle internal and external button (same action))
        if (pendingInternalButton || pendingExternalButton) {
            pendingInternalButton = false;
            pendingExternalButton = false;
            // drive
        }

        // pending rotary button
        if (pendingRotaryButton) {
            pendingRotaryButton = false;
            switch (currMenuSelected) {
                case MENU_PROFILE1:
                case MENU_PROFILE2:
                case MENU_PROFILE3:
                case MENU_PROFILE4:
                case MENU_PROFILE5:
                    break;

                case MENU_FORWARD:
                    break;

                case MENU_REVERSE:
                    break;

                case MENU_SETTINGS:
                    break;
            }
        }

        // figure out what to do with rotary direction
        if (pendingRotaryDirection != ROTARY_DIRECTION_NONE) {
            if ((pendingRotaryDirection == ROTARY_DIRECTION_LEFT) && (currMenuSelected > 1)) {
                currMenuSelected--;
            } else if ((pendingRotaryDirection == ROTARY_DIRECTION_RIGHT) && (currMenuSelected < MENU_MAX)) {
                currMenuSelected++;
            }
            pendingRotaryDirection = ROTARY_DIRECTION_NONE;
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
