#include <stdbool.h>
#include "app.h"
#include "appMain.h"
#include "io.h"
#include "rotary.h"
#include "ssd1306.h"
#include "watchdog.h"
#include "ticker.h"
#include "settings.h"

#if !defined(_XTAL_FREQ)
#error Must define _XTAL_FREQ
#endif

#define MENU_PUSH  1
#define MENU_HOLD  2
#define MENU_PULL  3
#define MENU_EXIT  4

#define MENU_MAX   4

void appProfile_drawProfileMenuDuration(uint8_t value, bool isLarge) {
    if (value < 10) {
        if (isLarge) {
            ssd1306_writeCharacter16(' ');
            ssd1306_writeCharacter16(' ');
            ssd1306_writeCharacter16(0x30 + value);
        } else {
            ssd1306_writeCharacter(' ');
            ssd1306_writeCharacter(' ');
            ssd1306_writeCharacter(0x30 + value);
        }
    } else if (value < 100) {
        uint8_t d2 = (value / 10);
        uint8_t d3 = (value % 10);
        if (isLarge) {
            ssd1306_writeCharacter16(' ');
            ssd1306_writeCharacter16(0x30 + d2);
            ssd1306_writeCharacter16(0x30 + d3);
        } else {
            ssd1306_writeCharacter(' ');
            ssd1306_writeCharacter(0x30 + d2);
            ssd1306_writeCharacter(0x30 + d3);
        }
    } else {
        uint8_t d1 = (value / 100);
        uint8_t d2 = ((value / 10) % 10);
        uint8_t d3 = (value % 10);
        if (isLarge) {
            ssd1306_writeCharacter16(0x30 + d1);
            ssd1306_writeCharacter16(0x30 + d2);
            ssd1306_writeCharacter16(0x30 + d3);
        } else {
            ssd1306_writeCharacter(0x30 + d1);
            ssd1306_writeCharacter(0x30 + d2);
            ssd1306_writeCharacter(0x30 + d3);
        }
    }
}

uint8_t appProfile_getNewValue(uint8_t value) {
    uint8_t currValue = value;
    uint8_t lastValue = 255;  // not a valid value

    while(true) {
        watchdog_clear();

        // figure if rotary button moved
        switch (rotary_getDirection()) {
            case ROTARY_DIRECTION_LEFT:
                if (currValue > 0) { currValue--; }
                break;
            case ROTARY_DIRECTION_RIGHT:
                if (currValue < 250) { currValue++; }
                break;
            default: break;
        }

        if (io_in_rotButton()) {
            while (io_in_rotButton()) { watchdog_clear(); }
            return currValue;
        }

        // draw menu
        if (ticker_hasTicked()) {  // but not too often
            if (currValue != lastValue) {
                lastValue = currValue;
                ssd1306_moveTo(3, 11);
                appProfile_drawProfileMenuDuration(currValue, true);
                ssd1306_writeLine16(" ms");
            }
        }
    }    
}

void execProfile(uint8_t index) {
    uint8_t lastMenuSelected = 0;
    uint8_t currMenuSelected = 1;

    ssd1306_clearAll();

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

        if (io_in_rotButton()) {
            switch (currMenuSelected) {
                case MENU_PUSH: {
                    while (io_in_rotButton()) { watchdog_clear(); }
                    ssd1306_moveTo(1, 1);
                    ssd1306_writeLine16("Push            ");
                    settings_setPush(index, appProfile_getNewValue(settings_getPush(index)));
                    settings_save();
                    lastMenuSelected = 0;  // reset display
                } break;

                case MENU_HOLD: {
                    while (io_in_rotButton()) { watchdog_clear(); }
                    ssd1306_moveTo(1, 1);
                    ssd1306_writeLine16("Hold            ");
                    settings_setHold(index, appProfile_getNewValue(settings_getHold(index)));
                    settings_save();
                    lastMenuSelected = 0;  // reset display
                } break;

                case MENU_PULL: {
                    while (io_in_rotButton()) { watchdog_clear(); }
                    ssd1306_moveTo(1, 1);
                    ssd1306_writeLine16("Pull            ");
                    settings_setPull(index, appProfile_getNewValue(settings_getPull(index)));
                    settings_save();
                    lastMenuSelected = 0;  // reset display
                } break;

                case MENU_EXIT: {
                    while (io_in_rotButton()) { watchdog_clear(); }
                    return;
                } break;
            }
        }

        // draw menu
        if (ticker_hasTicked()) {  // but not too often
            if (currMenuSelected != lastMenuSelected) {
                lastMenuSelected = currMenuSelected;
                ssd1306_displayNormal();
                ssd1306_moveTo(1, 1);
                switch (currMenuSelected) {
                    case MENU_PUSH:  ssd1306_writeLine16("Push            ");   ssd1306_writeLine("                "); ssd1306_writeLine("          "); appProfile_drawProfileMenuDuration(settings_getPush(index), false); ssd1306_writeLine(" ms"); break;
                    case MENU_HOLD:  ssd1306_writeLine16("Hold            ");   ssd1306_writeLine("                "); ssd1306_writeLine("          "); appProfile_drawProfileMenuDuration(settings_getHold(index), false); ssd1306_writeLine(" ms"); break;
                    case MENU_PULL:  ssd1306_writeLine16("Pull            ");   ssd1306_writeLine("                "); ssd1306_writeLine("          "); appProfile_drawProfileMenuDuration(settings_getPull(index), false); ssd1306_writeLine(" ms"); break;
                    case MENU_EXIT:  ssd1306_writeLine16("Exit            "); ssd1306_writeLine16("                "); break;
                    default:         ssd1306_clearAll(); break;
                }
            }
        }
    }
}
