/* Josip Medved <jmedved@jmedved.com> * www.medo64.com * MIT License */

/**
 * Handling SSD1306 display output
 *
 * Requires:
 *   _XTAL_FREQ <N>:               Frequency in Hz (only if not using external I2C master)
 *
 * Defines:
 *   _SSD1306_DISPLAY_ADDRESS <N>: I2C address; default is 0x3C
 *   _SSD1306_DISPLAY_HEIGHT <N>:  Display height; can be 32 or 64
 *   _SSD1306_DISPLAY_WIDTH <N>:   Display width; can be 64 or 128
 *   _SSD1306_DISPLAY_FLIP:        Flips screen to other direction
 *   _SSD1306_FONT_8x8:            Use 8-pixel high text
 *   _SSD1306_FONT_8x8_LOW:        Include lower 32 ASCII control characters
 *   _SSD1306_FONT_8x8_HIGH:       Include upper 128 CP437 ASCII characters
 *   _SSD1306_FONT_8x16:           Use 16-pixel high text
 *   _SSD1306_FONT_8x16_LOW:       Include lower 32 ASCII control characters
 *   _SSD1306_FONT_8x16_HIGH:      Include upper 128 CP437 ASCII characters
 *   _SSD1306_CONTROL_DISPLAY:     Allows display control (displayOff, displayOn)
 *   _SSD1306_CONTROL_INVERT:      Allows display control (displayInvert, displayNormal)
 *   _SSD1306_CONTROL_FLIP:        Allows display control (displayFlip)
 *   _SSD1306_CONTROL_CONTRAST:    Allows contrast control (setContrast)
 *   _SSD1306_EXTERNAL_I2C_MASTER: Uses external I2C implementation instead of the built-in one
 *   _SSD1306_CUSTOM_INIT:         Uses customizable initialization function
 */
// 2024-09-02: Refactored to work with OLED firmware
// 2023-10-10: Expanded functionality
// 2023-10-14: Internal I2C support

#pragma once

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "app.h"

#if !defined(_16F1454) && !defined(_16F1455)
    #error "Unsupported device"
#endif

#if !defined(_SSD1306_FONT_8x8) & !defined(_SSD1306_FONT_8x16)  // at least one font has to be supported
    #define _SSD1306_FONT_8x8
#endif


/** Initializes Display. */
#if defined(_SSD1306_CUSTOM_INIT)
    void ssd1306_init(const uint8_t address, const uint8_t baudRateCounter, const uint8_t width, const uint8_t height);
#else
    void ssd1306_init(void);

    #if !defined(_SSD1306_DISPLAY_ADDRESS)
        #define _SSD1306_DISPLAY_ADDRESS  0x3C
    #endif

    #if !defined(_SSD1306_DISPLAY_HEIGHT)
        #define _SSD1306_DISPLAY_HEIGHT   32
    #elif (_SSD1306_DISPLAY_HEIGHT != 32) && (_SSD1306_DISPLAY_HEIGHT != 64)
        #error SSD1306 display height not supported
    #endif

    #if !defined(_SSD1306_DISPLAY_WIDTH)
        #define _SSD1306_DISPLAY_WIDTH   128
    #elif (_SSD1306_DISPLAY_WIDTH != 64) && (_SSD1306_DISPLAY_HEIGHT != 128)
        #error SSD1306 display width not supported
    #endif
#endif

/** Turns display off. */
#if defined(_SSD1306_CONTROL_DISPLAY)
    void ssd1306_displayOff(void);
#endif

/** Turns display on. */
#if defined(_SSD1306_CONTROL_DISPLAY)
    void ssd1306_displayOn(void);
#endif

/** Turns on display inversion. */
#if defined(_SSD1306_CONTROL_INVERT)
    void ssd1306_displayInvert(void);
#endif

/** Turns off display inversion. */
#if defined(_SSD1306_CONTROL_INVERT)
    void ssd1306_displayNormal(void);
#endif

/** Flips display orientation. */
#if defined(_SSD1306_CONTROL_FLIP)
    void ssd1306_displayFlip(bool flipped);
#endif

/** Sets contrast value. */
#if defined(_SSD1306_CONTROL_CONTRAST)
    void ssd1306_setContrast(const uint8_t value);
#endif

/** Sets column and row to be used (at 8x8 resolution). */
bool ssd1306_moveTo(const uint8_t row, const uint8_t column);


/** Moves cursor to the first column of the next row. */
#if defined(_SSD1306_FONT_8x8)
    bool ssd1306_moveToNextRow(void);
#endif

/** Moves cursor to the first column of the next row (16-pixel characters). */
#if defined(_SSD1306_FONT_8x16)
    bool ssd1306_moveToNextRow16(void);
#endif


/** Clear display content. */
void ssd1306_clearAll(void);


/** Clear remaining. */
#if defined(_SSD1306_FONT_8x8)
    void ssd1306_clearRemaining(void);
#endif

/** Clear remaining. */
#if defined(_SSD1306_FONT_8x16)
    void ssd1306_clearRemaining16(void);
#endif


/** Clear display content of a single row. */
#if defined(_SSD1306_FONT_8x8)
    bool ssd1306_clearRow(const uint8_t row);
#endif

/** Clear display content of a single row. */
#if defined(_SSD1306_FONT_8x16)
    bool ssd1306_clearRow16(const uint8_t row);
#endif


/** Writes custom 8x8 character at the current position */
bool ssd1306_drawCharacter(const uint8_t* data, const uint8_t count);


/** Writes 8x8 character at the current position */
#if defined(_SSD1306_FONT_8x8)
    bool ssd1306_writeCharacter(const char value);
#endif

/** Writes 8x16 character at the current position */
#if defined(_SSD1306_FONT_8x16)
    bool ssd1306_writeCharacter16(const char value);
#endif


/** Writes 8x8 text at the current position */
#if defined(_SSD1306_FONT_8x8)
    bool ssd1306_writeText(const char* text);
#endif

/** Writes 8x8 text at the current position and moves to the next line */
#if defined(_SSD1306_FONT_8x8)
    bool ssd1306_writeLine(const char* text);
#endif

/** Writes 8x16 text at the current position */
#if defined(_SSD1306_FONT_8x16)
    bool ssd1306_writeText16(const char* text);
#endif

/** Writes 8x16 text at the current position and moves to the next line */
#if defined(_SSD1306_FONT_8x16)
    bool ssd1306_writeLine16(const char* text);
#endif
