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

#define MENU_DISPLAY_FLIP  1
#define MENU_DRIVE_FLIP    2
#define MENU_EXIT          3

#define MENU_MAX       3

void execSettings(void) {
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
                case MENU_DISPLAY_FLIP: {
                    while (io_in_rotButton()) { watchdog_clear(); }
                    settings_setIsDisplayFlipped(!settings_getIsDisplayFlipped());
                    settings_save();
                    ssd1306_displayFlip(settings_getIsDisplayFlipped());
                    lastMenuSelected = 0;  // reset display
                } break;

                case MENU_DRIVE_FLIP: {
                    while (io_in_rotButton()) { watchdog_clear(); }
                    settings_setIsDriveFlipped(!settings_getIsDriveFlipped());
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
                    case MENU_DISPLAY_FLIP:  ssd1306_writeLine16("Display flip    "); ssd1306_writeLine16(settings_getIsDisplayFlipped() ? "             Yes" : "              No"); break;
                    case MENU_DRIVE_FLIP:    ssd1306_writeLine16("Drive flip      "); ssd1306_writeLine16(settings_getIsDriveFlipped()   ? "             Yes" : "              No"); break;
                    case MENU_EXIT:          ssd1306_writeLine16("Exit            "); ssd1306_writeLine16("                "); break;
                    default:                 ssd1306_clearAll(); break;
                }
            }
        }
    }
}
