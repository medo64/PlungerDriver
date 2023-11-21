#include <stdbool.h>
#include "app.h"
#include "appMain.h"
#include "appSettings.h"
#include "io.h"
#include "rotary.h"
#include "ssd1306.h"
#include "watchdog.h"
#include "ticker.h"
#include "motor.h"
#include "settings.h"

#if !defined(_XTAL_FREQ)
#error Must define _XTAL_FREQ
#endif

#define MENU_PROFILE1  1
#define MENU_PROFILE2  2
#define MENU_PROFILE3  3
#define MENU_PROFILE4  4
#define MENU_PROFILE5  5
#define MENU_FORWARD   6
#define MENU_REVERSE   7
#define MENU_SETTINGS  8

#define MENU_MAX       8

void drawProfileMenuDuration(uint8_t value) {
    if (value < 10) {
        ssd1306_writeCharacter16(' ');
        ssd1306_writeCharacter16(' ');
        ssd1306_writeCharacter16(0x30 + value);
    } else if (value < 100) {
        uint8_t d2 = (value / 10);
        uint8_t d3 = (value % 10);
        ssd1306_writeCharacter16(' ');
        ssd1306_writeCharacter16(0x30 + d2);
        ssd1306_writeCharacter16(0x30 + d3);
    } else {
        uint8_t d1 = (value / 100);
        uint8_t d2 = ((value / 10) % 10);
        uint8_t d3 = (value % 10);
        ssd1306_writeCharacter16(0x30 + d1);
        ssd1306_writeCharacter16(0x30 + d2);
        ssd1306_writeCharacter16(0x30 + d3);
    }
}

void drawProfileMenuItem(uint8_t index) {
    if (index < 9) {
        ssd1306_write16("Profile ");
        ssd1306_writeCharacter16(0x30 + index + 1);
        ssd1306_writeLine16("       ");
    } else {
        ssd1306_writeLine16("Profile 10      ");
    }
    ssd1306_writeCharacter16(' ');
    ssd1306_writeCharacter16(' ');
    drawProfileMenuDuration(settings_getPush(index));
    ssd1306_writeCharacter(0x1A);
    ssd1306_writeCharacter16(' ');
    drawProfileMenuDuration(settings_getHold(index));
    ssd1306_writeCharacter(0x1D);
    ssd1306_writeCharacter16(' ');
    drawProfileMenuDuration(settings_getPull(index));
    ssd1306_writeCharacter(0x1B);
}

void execMain(void) {
    uint8_t lastMenuSelected = 0;
    uint8_t currMenuSelected = 1;

    ssd1306_displayFlip(settings_getIsDisplayFlipped());

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
                if (io_in_internalButton() || io_in_externalButton()) {
                    ssd1306_displayInvert();
                    motor_setForward();
                    for (uint8_t i = 0; i < settings_getPush(currMenuSelected - MENU_PROFILE1); i++) {
                        __delay_ms(1);
                    }
                    motor_setBrake();
                    for (uint8_t i = 0; i < settings_getHold(currMenuSelected - MENU_PROFILE1); i++) {
                        __delay_ms(1);
                    }
                    motor_setReverse();
                    for (uint8_t i = 0; i < settings_getPull(currMenuSelected - MENU_PROFILE1); i++) {
                        __delay_ms(1);
                    }
                    motor_setSleep();
                    ssd1306_displayNormal();
                    while (io_in_internalButton() || io_in_externalButton()) { watchdog_clear(); }
                } else if (io_in_rotButton()) {  // go into profile settings
                    // TODO: Settings
                }
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
                if (io_in_rotButton()) {  // go into settings
                    while (io_in_rotButton()) { watchdog_clear(); }
                    execSettings();
                    lastMenuSelected = 0;  // redraw
                }
                break;
        }

        // draw menu
        if (ticker_hasTicked()) {  // but not too often
            if (currMenuSelected != lastMenuSelected) {
                lastMenuSelected = currMenuSelected;
                ssd1306_displayNormal();
                ssd1306_moveTo(1, 1);
                switch (currMenuSelected) {
                    case MENU_PROFILE1: drawProfileMenuItem(0); break;
                    case MENU_PROFILE2: drawProfileMenuItem(1); break;
                    case MENU_PROFILE3: drawProfileMenuItem(2); break;
                    case MENU_PROFILE4: drawProfileMenuItem(3); break;
                    case MENU_PROFILE5: drawProfileMenuItem(4); break;
                    case MENU_FORWARD:  ssd1306_writeLine16("Forward         "); ssd1306_writeLine16("                "); break;
                    case MENU_REVERSE:  ssd1306_writeLine16("Reverse         "); ssd1306_writeLine16("                "); break;
                    case MENU_SETTINGS: ssd1306_writeLine16("Settings        "); ssd1306_writeLine16("                "); break;
                    default:            ssd1306_clearAll(); break;
                }
            }
        }
    }
}
