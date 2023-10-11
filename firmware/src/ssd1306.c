/* Josip Medved <jmedved@jmedved.com> * www.medo64.com * MIT License */

#include <xc.h>
#include <stdint.h>
#include "i2c_master.h"
#include "ssd1306.h"

#define SSD1306_SET_LOWER_START_COLUMN_ADDRESS       0x00
#define SSD1306_SET_UPPER_START_COLUMN_ADDRESS       0x10
#define SSD1306_SET_MEMORY_ADDRESSING_MODE           0x20
#define SSD1306_SET_COLUMN_ADDRESS                   0x21
#define SSD1306_SET_PAGE_ADDRESS                     0x22
#define SSD1306_SET_DISPLAY_START_LINE               0x40
#define SSD1306_SET_CONTRAST_CONTROL                 0x81
#define SSD1306_SET_CHARGE_PUMP                      0x8D
#define SSD1306_SET_SEGMENT_REMAP_COL0               0xA0
#define SSD1306_SET_SEGMENT_REMAP_COL127             0xA1
#define SSD1306_ENTIRE_DISPLAY_ON                    0xA4
#define SSD1306_ENTIRE_DISPLAY_ON_FORCED             0xA5
#define SSD1306_SET_NORMAL_DISPLAY                   0xA6
#define SSD1306_SET_INVERSE_DISPLAY                  0xA7
#define SSD1306_SET_DISPLAY_OFF                      0xAE
#define SSD1306_SET_DISPLAY_ON                       0xAF
#define SSD1306_SET_MULTIPLEX_RATIO                  0xA8
#define SSD1306_SET_PAGE_START_ADDRESS               0xB0
#define SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_INC    0xC0
#define SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_DEC    0xC8
#define SSD1306_SET_DISPLAY_OFFSET                   0xD3
#define SSD1306_SET_DISPLAY_CLOCK_DIVIDE_RATIO       0xD5
#define SSD1306_SET_PRECHARGE_PERIOD                 0xD9
#define SSD1306_SET_COM_PINS_HARDWARE_CONFIGURATION  0xDA
#define SSD1306_SET_VCOMH_DESELECT_LEVEL             0xDB

#if defined(_SSD1306_FONT_8x8)
    extern const uint8_t font_8x8[];
#endif
#if defined(_SSD1306_FONT_8x16)
    extern const uint8_t font_8x16[];
#endif

#define SSD1306_DISPLAY_COLS  (_SSD1306_DISPLAY_WIDTH / 8)
#define SSD1306_DISPLAY_ROWS  (_SSD1306_DISPLAY_HEIGHT / 8)

uint8_t currentRow;
uint8_t currentColumn;


void writeRawCommand1(const uint8_t value1) {
   i2c_master_startWrite(_SSD1306_DISPLAY_ADDRESS);
   i2c_master_writeByte(0x00);
   i2c_master_writeByte(value1);
   i2c_master_stop();
}

void writeRawCommand2(const uint8_t value1, const uint8_t value2) {
   i2c_master_startWrite(_SSD1306_DISPLAY_ADDRESS);
   i2c_master_writeByte(0x00);
   i2c_master_writeByte(value1);
   i2c_master_writeByte(value2);
   i2c_master_stop();
}

void writeRawCommand3(const uint8_t value1, const uint8_t value2, const uint8_t value3) {
   i2c_master_startWrite(_SSD1306_DISPLAY_ADDRESS);
   i2c_master_writeByte(0x00);
   i2c_master_writeByte(value1);
   i2c_master_writeByte(value2);
   i2c_master_writeByte(value3);
   i2c_master_stop();
}

void writeRawData(const uint8_t *value, const uint8_t count) {
   i2c_master_startWrite(_SSD1306_DISPLAY_ADDRESS);
   i2c_master_writeByte(0x40);
   for (uint8_t i = 0; i < count; i++) {
       i2c_master_writeByte(*value);
       value++;
   }
   i2c_master_stop();
}

void writeRawDataZeros(const uint8_t count) {
   i2c_master_startWrite(_SSD1306_DISPLAY_ADDRESS);
   i2c_master_writeByte(0x40);
   for (uint8_t i = 0; i < count; i++) {
       i2c_master_writeByte(0x00);
   }
   i2c_master_stop();
}


void ssd1306_init() {
    writeRawCommand1(SSD1306_SET_DISPLAY_OFF);                                    // Set Display Off
    writeRawCommand2(SSD1306_SET_DISPLAY_CLOCK_DIVIDE_RATIO, 0x80);               // Set Display Clock Divide Ratio/Oscillator Frequency
    writeRawCommand2(SSD1306_SET_MULTIPLEX_RATIO, _SSD1306_DISPLAY_HEIGHT - 1);   // Set Multiplex Ratio (line count - 1)
    writeRawCommand2(SSD1306_SET_DISPLAY_OFFSET, 0x00);                           // Set Display Offset
    writeRawCommand1(SSD1306_SET_DISPLAY_START_LINE);                             // Set Display Start Line 
    writeRawCommand2(SSD1306_SET_CHARGE_PUMP, 0x14);                              // Set Charge Pump (0x10 external vcc, 0x14 internal vcc)
    #if defined(_SSD1306_DISPLAY_FLIP)
        writeRawCommand1(SSD1306_SET_SEGMENT_REMAP_COL127);                       // Set Segment Re-Map
        writeRawCommand1(SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_DEC);              // Set COM Output Scan Direction
    #else
        writeRawCommand1(SSD1306_SET_SEGMENT_REMAP_COL0);                         // Set Segment Re-Map
        writeRawCommand1(SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_INC);              // Set COM Output Scan Direction
    #endif
    #if (_SSD1306_DISPLAY_HEIGHT == 32)
        writeRawCommand2(SSD1306_SET_COM_PINS_HARDWARE_CONFIGURATION, 0x02);      // Set COM Pins Hardware Configuration (0x02 128x32)
        writeRawCommand2(SSD1306_SET_CONTRAST_CONTROL, 0x8F);                     // Set Contrast Control (0x8F 128x32; external vcc; internal vcc)
    #else
        writeRawCommand2(SSD1306_SET_COM_PINS_HARDWARE_CONFIGURATION, 0x12);      // Set COM Pins Hardware Configuration (0x12 128x64)
        writeRawCommand2(SSD1306_SET_CONTRAST_CONTROL, 0xCF);                     // Set Contrast Control (0xCF 128x64; external vcc; internal vcc)
    #endif
    writeRawCommand2(SSD1306_SET_PRECHARGE_PERIOD, 0xF1);                         // Set Pre-Charge Period (0x22 external vcc; 0xF1 internal vcc)
    writeRawCommand2(SSD1306_SET_VCOMH_DESELECT_LEVEL, 0x40);                     // Set VCOMH Deselect Level
    writeRawCommand1(SSD1306_ENTIRE_DISPLAY_ON);                                  // Set Entire Display On/Off
    writeRawCommand1(SSD1306_SET_NORMAL_DISPLAY);                                 // Set Normal Display

    writeRawCommand2(SSD1306_SET_MEMORY_ADDRESSING_MODE, 0b10);                   // Set Page addressing mode

    ssd1306_clearAll();

    writeRawCommand1(SSD1306_SET_DISPLAY_ON);                                     // Set Display On
}


#if !defined(_SSD1306_NO_DISPLAY_CONTROL)

void ssd1306_displayOff(void) {
    writeRawCommand1(SSD1306_SET_DISPLAY_OFF);
}

void ssd1306_displayOn(void) {
    writeRawCommand1(SSD1306_SET_DISPLAY_ON);
}

void ssd1306_displayInvert(void) {
    writeRawCommand1(SSD1306_SET_INVERSE_DISPLAY);
}

void ssd1306_displayNormal(void) {
    writeRawCommand1(SSD1306_SET_NORMAL_DISPLAY);
}

void ssd1306_setContrast(const uint8_t value) {
    writeRawCommand2(SSD1306_SET_CONTRAST_CONTROL, value);
}

#endif


void ssd1306_clearAll(void) {
    for (uint8_t i = 0; i < SSD1306_DISPLAY_COLS; i++) {
        writeRawCommand1(SSD1306_SET_PAGE_START_ADDRESS | i);
        writeRawDataZeros(_SSD1306_DISPLAY_WIDTH);
    }
    ssd1306_moveTo(1, 1);
}


#if defined(_SSD1306_FONT_8x8)
void ssd1306_clearRemaining(void) {
    uint8_t columnCount = (uint8_t)((SSD1306_DISPLAY_COLS - currentColumn) << 3);
    uint8_t nextColumn = currentColumn + 1;
    uint8_t nextRow = currentRow + 1;
    writeRawDataZeros(columnCount);
    ssd1306_moveTo(nextRow, nextColumn);
}
#endif

#if defined(_SSD1306_FONT_8x16)
void ssd1306_clearRemaining16(void) {
    uint8_t columnCount = (uint8_t)((SSD1306_DISPLAY_COLS - currentColumn) << 3);
    uint8_t nextColumn = currentColumn + 1;
    uint8_t nextRow = currentRow + 1;
    writeRawDataZeros(columnCount);
    ssd1306_moveTo(nextRow + 1, nextColumn);
    writeRawDataZeros(columnCount);
    ssd1306_moveTo(nextRow, nextColumn);
}
#endif


#if defined(_SSD1306_FONT_8x8)
bool ssd1306_clearRow(const uint8_t row) {
    if (ssd1306_moveTo(row, 1)) {
        writeRawDataZeros(_SSD1306_DISPLAY_WIDTH);
        return true;
    }
    return false;
}
#endif

#if defined(_SSD1306_FONT_8x16)
bool ssd1306_clearRow16(const uint8_t row) {
    if (ssd1306_moveTo(row, 1)) {
        writeRawDataZeros(_SSD1306_DISPLAY_WIDTH);
        if (ssd1306_moveTo(row + 1, 1)) {
            writeRawDataZeros(_SSD1306_DISPLAY_WIDTH);
            return true;
        }
    }
    return false;
}
#endif


bool ssd1306_moveTo(const uint8_t row, const uint8_t column) {
    if ((row <= SSD1306_DISPLAY_ROWS) && (column <= SSD1306_DISPLAY_COLS)) {
        uint8_t newRow = (row == 0) ? currentRow : row - 1;
        uint8_t newColumn = (column == 0) ? currentColumn : column - 1;
        uint8_t newColumnL = (newColumn << 3) & 0x0F;
        uint8_t newColumnH = (newColumn >> 1) & 0x0F;
        writeRawCommand1(SSD1306_SET_PAGE_START_ADDRESS | newRow);
        writeRawCommand1(SSD1306_SET_LOWER_START_COLUMN_ADDRESS | newColumnL);
        writeRawCommand1(SSD1306_SET_UPPER_START_COLUMN_ADDRESS | newColumnH);
        currentRow = newRow;
        currentColumn = newColumn;
        return true;
    }
    return false;
}


#if defined(_SSD1306_FONT_8x8)
bool ssd1306_moveToNextRow(void) {
    if (currentRow >= SSD1306_DISPLAY_ROWS - 1) { return false; }
    uint8_t newRow = currentRow + 1;
    writeRawCommand1(SSD1306_SET_PAGE_START_ADDRESS | newRow);
    writeRawCommand1(SSD1306_SET_LOWER_START_COLUMN_ADDRESS);
    writeRawCommand1(SSD1306_SET_UPPER_START_COLUMN_ADDRESS);
    currentRow = newRow;
    currentColumn = 0;
    return true;
}
#endif

#if defined(_SSD1306_FONT_8x16)
bool ssd1306_moveToNextRow16(void) {
    if (currentRow >= SSD1306_DISPLAY_ROWS - 1) { return false; }
    uint8_t newRow = currentRow + 2;
    writeRawCommand1(SSD1306_SET_PAGE_START_ADDRESS | newRow);
    writeRawCommand1(SSD1306_SET_LOWER_START_COLUMN_ADDRESS);
    writeRawCommand1(SSD1306_SET_UPPER_START_COLUMN_ADDRESS);
    currentRow = newRow;
    currentColumn = 0;
    return true;
}
#endif


bool ssd1306_drawCharacter(const uint8_t* data, const uint8_t count) {
    if ((count != 8) && (count != 16)) { return false; }  // must be 8x8 or 8x16 pixels - 8/16 bytes
    if (currentColumn >= SSD1306_DISPLAY_COLS) { return false; }

    if (count >= 16) {
        writeRawCommand1(SSD1306_SET_PAGE_START_ADDRESS | (currentRow + 1));
        writeRawData(data + 8, 8);

        uint8_t currentColumnLow = (currentColumn << 3) & 0x0F;
        uint8_t currentColumnHigh = (currentColumn >> 1) & 0x0F;
        writeRawCommand1(SSD1306_SET_PAGE_START_ADDRESS | currentRow);
        writeRawCommand1(SSD1306_SET_LOWER_START_COLUMN_ADDRESS | currentColumnLow);
        writeRawCommand1(SSD1306_SET_UPPER_START_COLUMN_ADDRESS | currentColumnHigh);
        writeRawData(data, 8);
    } else {
        writeRawData(data, 8);
    }
    currentColumn++;

    return true;
}

#if defined(_SSD1306_FONT_8x8)
bool ssd1306_writeCharacter(const char value) {
    if ((value < 32) || (value > 126)) {
        return ssd1306_drawCharacter(&font_8x8[0], 8);
    } else {
        uint16_t offset = (uint16_t)((value - 32) << 3);  // *8
        return ssd1306_drawCharacter(&font_8x8[offset], 8);
    }
}
#endif

#if defined(_SSD1306_FONT_8x16)
bool ssd1306_writeCharacter16(const char value) {
    if ((value < 32) || (value > 126)) {
        return ssd1306_drawCharacter(&font_8x16[0], 16);
    } else {
        uint16_t offset = (uint16_t)((value - 32) << 4);  // *16
        return ssd1306_drawCharacter(&font_8x16[offset], 16);
    }
}
#endif

#if defined(_SSD1306_FONT_8x8)
bool ssd1306_write(const char* text) {
    bool ok = true;
    while (*text != 0) {
        ok &= ssd1306_writeCharacter(*text);
        text++;
    }
    return ok;
}

bool ssd1306_writeLine(const char* text) {
    bool ok = ssd1306_write(text);
    ssd1306_moveToNextRow();
    return ok;
}
#endif

#if defined(_SSD1306_FONT_8x16)
bool ssd1306_write16(const char* text) {
    bool ok = true;
    while (*text != 0) {
        ok &= ssd1306_writeCharacter16(*text);
        text++;
    }
    return ok;
}

bool ssd1306_writeLine16(const char* text) {
    bool ok = ssd1306_write16(text);
    ssd1306_moveToNextRow16();
    return ok;
}
#endif


#if defined(_SSD1306_FONT_8x8)
const uint8_t font_8x8[] = {
          //     0x20
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // !   0x21
    0x00, // ░░░░░░░░
    0x06, // ░░░░░██░
    0x5F, // ░█░█████
    0x5F, // ░█░█████
    0x06, // ░░░░░██░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // "   0x22
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // #   0x23
    0x14, // ░░░█░█░░
    0x7F, // ░███████
    0x7F, // ░███████
    0x14, // ░░░█░█░░
    0x7F, // ░███████
    0x7F, // ░███████
    0x14, // ░░░█░█░░
    0x00, // ░░░░░░░░
          // $   0x24
    0x24, // ░░█░░█░░
    0x2E, // ░░█░███░
    0x6B, // ░██░█░██
    0x6B, // ░██░█░██
    0x3A, // ░░███░█░
    0x12, // ░░░█░░█░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // %   0x25
    0x46, // ░█░░░██░
    0x66, // ░██░░██░
    0x30, // ░░██░░░░
    0x18, // ░░░██░░░
    0x0C, // ░░░░██░░
    0x66, // ░██░░██░
    0x62, // ░██░░░█░
    0x00, // ░░░░░░░░
          // &   0x26
    0x30, // ░░██░░░░
    0x7A, // ░████░█░
    0x4F, // ░█░░████
    0x5D, // ░█░███░█
    0x37, // ░░██░███
    0x7A, // ░████░█░
    0x48, // ░█░░█░░░
    0x00, // ░░░░░░░░
          // '   0x27
    0x04, // ░░░░░█░░
    0x07, // ░░░░░███
    0x03, // ░░░░░░██
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // (   0x28
    0x00, // ░░░░░░░░
    0x1C, // ░░░███░░
    0x3E, // ░░█████░
    0x63, // ░██░░░██
    0x41, // ░█░░░░░█
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // )   0x29
    0x00, // ░░░░░░░░
    0x41, // ░█░░░░░█
    0x63, // ░██░░░██
    0x3E, // ░░█████░
    0x1C, // ░░░███░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // *   0x2A
    0x08, // ░░░░█░░░
    0x2A, // ░░█░█░█░
    0x3E, // ░░█████░
    0x1C, // ░░░███░░
    0x1C, // ░░░███░░
    0x3E, // ░░█████░
    0x2A, // ░░█░█░█░
    0x08, // ░░░░█░░░
          // +   0x2B
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x3E, // ░░█████░
    0x3E, // ░░█████░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // ,   0x2C
    0x00, // ░░░░░░░░
    0x80, // █░░░░░░░
    0xE0, // ███░░░░░
    0x60, // ░██░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // -   0x2D
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // .   0x2E
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x60, // ░██░░░░░
    0x60, // ░██░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // /   0x2F
    0x60, // ░██░░░░░
    0x30, // ░░██░░░░
    0x18, // ░░░██░░░
    0x0C, // ░░░░██░░
    0x06, // ░░░░░██░
    0x03, // ░░░░░░██
    0x01, // ░░░░░░░█
    0x00, // ░░░░░░░░
          // 0   0x30
    0x3E, // ░░█████░
    0x7F, // ░███████
    0x71, // ░███░░░█
    0x59, // ░█░██░░█
    0x4D, // ░█░░██░█
    0x7F, // ░███████
    0x3E, // ░░█████░
    0x00, // ░░░░░░░░
          // 1   0x31
    0x40, // ░█░░░░░░
    0x42, // ░█░░░░█░
    0x7F, // ░███████
    0x7F, // ░███████
    0x40, // ░█░░░░░░
    0x40, // ░█░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // 2   0x32
    0x62, // ░██░░░█░
    0x73, // ░███░░██
    0x59, // ░█░██░░█
    0x49, // ░█░░█░░█
    0x6F, // ░██░████
    0x66, // ░██░░██░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // 3   0x33
    0x22, // ░░█░░░█░
    0x63, // ░██░░░██
    0x49, // ░█░░█░░█
    0x49, // ░█░░█░░█
    0x7F, // ░███████
    0x36, // ░░██░██░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // 4   0x34
    0x18, // ░░░██░░░
    0x1C, // ░░░███░░
    0x16, // ░░░█░██░
    0x53, // ░█░█░░██
    0x7F, // ░███████
    0x7F, // ░███████
    0x50, // ░█░█░░░░
    0x00, // ░░░░░░░░
          // 5   0x35
    0x27, // ░░█░░███
    0x67, // ░██░░███
    0x45, // ░█░░░█░█
    0x45, // ░█░░░█░█
    0x7D, // ░█████░█
    0x39, // ░░███░░█
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // 6   0x36
    0x3C, // ░░████░░
    0x7E, // ░██████░
    0x4B, // ░█░░█░██
    0x49, // ░█░░█░░█
    0x79, // ░████░░█
    0x30, // ░░██░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // 7   0x37
    0x03, // ░░░░░░██
    0x03, // ░░░░░░██
    0x71, // ░███░░░█
    0x79, // ░████░░█
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // 8   0x38
    0x36, // ░░██░██░
    0x7F, // ░███████
    0x49, // ░█░░█░░█
    0x49, // ░█░░█░░█
    0x7F, // ░███████
    0x36, // ░░██░██░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // 9   0x39
    0x06, // ░░░░░██░
    0x4F, // ░█░░████
    0x49, // ░█░░█░░█
    0x69, // ░██░█░░█
    0x3F, // ░░██████
    0x1E, // ░░░████░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // :   0x3A
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x66, // ░██░░██░
    0x66, // ░██░░██░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // ;   0x3B
    0x00, // ░░░░░░░░
    0x80, // █░░░░░░░
    0xE6, // ███░░██░
    0x66, // ░██░░██░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // <   0x3C
    0x08, // ░░░░█░░░
    0x1C, // ░░░███░░
    0x36, // ░░██░██░
    0x63, // ░██░░░██
    0x41, // ░█░░░░░█
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // =   0x3D
    0x24, // ░░█░░█░░
    0x24, // ░░█░░█░░
    0x24, // ░░█░░█░░
    0x24, // ░░█░░█░░
    0x24, // ░░█░░█░░
    0x24, // ░░█░░█░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // >   0x3E
    0x00, // ░░░░░░░░
    0x41, // ░█░░░░░█
    0x63, // ░██░░░██
    0x36, // ░░██░██░
    0x1C, // ░░░███░░
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // ?   0x3F
    0x02, // ░░░░░░█░
    0x03, // ░░░░░░██
    0x51, // ░█░█░░░█
    0x59, // ░█░██░░█
    0x0F, // ░░░░████
    0x06, // ░░░░░██░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // @   0x40
    0x3E, // ░░█████░
    0x7F, // ░███████
    0x41, // ░█░░░░░█
    0x5D, // ░█░███░█
    0x5D, // ░█░███░█
    0x1F, // ░░░█████
    0x1E, // ░░░████░
    0x00, // ░░░░░░░░
          // A   0x41
    0x7C, // ░█████░░
    0x7E, // ░██████░
    0x13, // ░░░█░░██
    0x13, // ░░░█░░██
    0x7E, // ░██████░
    0x7C, // ░█████░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // B   0x42
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x49, // ░█░░█░░█
    0x49, // ░█░░█░░█
    0x7F, // ░███████
    0x36, // ░░██░██░
    0x00, // ░░░░░░░░
          // C   0x43
    0x1C, // ░░░███░░
    0x3E, // ░░█████░
    0x63, // ░██░░░██
    0x41, // ░█░░░░░█
    0x41, // ░█░░░░░█
    0x63, // ░██░░░██
    0x22, // ░░█░░░█░
    0x00, // ░░░░░░░░
          // D   0x44
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x41, // ░█░░░░░█
    0x63, // ░██░░░██
    0x3E, // ░░█████░
    0x1C, // ░░░███░░
    0x00, // ░░░░░░░░
          // E   0x45
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x49, // ░█░░█░░█
    0x5D, // ░█░███░█
    0x41, // ░█░░░░░█
    0x63, // ░██░░░██
    0x00, // ░░░░░░░░
          // F   0x46
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x49, // ░█░░█░░█
    0x1D, // ░░░███░█
    0x01, // ░░░░░░░█
    0x03, // ░░░░░░██
    0x00, // ░░░░░░░░
          // G   0x47
    0x1C, // ░░░███░░
    0x3E, // ░░█████░
    0x63, // ░██░░░██
    0x41, // ░█░░░░░█
    0x51, // ░█░█░░░█
    0x73, // ░███░░██
    0x72, // ░███░░█░
    0x00, // ░░░░░░░░
          // H   0x48
    0x7F, // ░███████
    0x7F, // ░███████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x7F, // ░███████
    0x7F, // ░███████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // I   0x49
    0x00, // ░░░░░░░░
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x41, // ░█░░░░░█
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // J   0x4A
    0x30, // ░░██░░░░
    0x70, // ░███░░░░
    0x40, // ░█░░░░░░
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x3F, // ░░██████
    0x01, // ░░░░░░░█
    0x00, // ░░░░░░░░
          // K   0x4B
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x08, // ░░░░█░░░
    0x1C, // ░░░███░░
    0x77, // ░███░███
    0x63, // ░██░░░██
    0x00, // ░░░░░░░░
          // L   0x4C
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x41, // ░█░░░░░█
    0x40, // ░█░░░░░░
    0x60, // ░██░░░░░
    0x70, // ░███░░░░
    0x00, // ░░░░░░░░
          // M   0x4D
    0x7F, // ░███████
    0x7F, // ░███████
    0x0E, // ░░░░███░
    0x1C, // ░░░███░░
    0x0E, // ░░░░███░
    0x7F, // ░███████
    0x7F, // ░███████
    0x00, // ░░░░░░░░
          // N   0x4E
    0x7F, // ░███████
    0x7F, // ░███████
    0x06, // ░░░░░██░
    0x0C, // ░░░░██░░
    0x18, // ░░░██░░░
    0x7F, // ░███████
    0x7F, // ░███████
    0x00, // ░░░░░░░░
          // O   0x4F
    0x1C, // ░░░███░░
    0x3E, // ░░█████░
    0x63, // ░██░░░██
    0x41, // ░█░░░░░█
    0x63, // ░██░░░██
    0x3E, // ░░█████░
    0x1C, // ░░░███░░
    0x00, // ░░░░░░░░
          // P   0x50
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x49, // ░█░░█░░█
    0x09, // ░░░░█░░█
    0x0F, // ░░░░████
    0x06, // ░░░░░██░
    0x00, // ░░░░░░░░
          // Q   0x51
    0x1E, // ░░░████░
    0x3F, // ░░██████
    0x21, // ░░█░░░░█
    0x71, // ░███░░░█
    0x7F, // ░███████
    0x5E, // ░█░████░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // R   0x52
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x09, // ░░░░█░░█
    0x19, // ░░░██░░█
    0x7F, // ░███████
    0x66, // ░██░░██░
    0x00, // ░░░░░░░░
          // S   0x53
    0x26, // ░░█░░██░
    0x6F, // ░██░████
    0x4D, // ░█░░██░█
    0x59, // ░█░██░░█
    0x73, // ░███░░██
    0x32, // ░░██░░█░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // T   0x54
    0x03, // ░░░░░░██
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x41, // ░█░░░░░█
    0x03, // ░░░░░░██
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // U   0x55
    0x7F, // ░███████
    0x7F, // ░███████
    0x40, // ░█░░░░░░
    0x40, // ░█░░░░░░
    0x7F, // ░███████
    0x7F, // ░███████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // V   0x56
    0x1F, // ░░░█████
    0x3F, // ░░██████
    0x60, // ░██░░░░░
    0x60, // ░██░░░░░
    0x3F, // ░░██████
    0x1F, // ░░░█████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // W   0x57
    0x7F, // ░███████
    0x7F, // ░███████
    0x30, // ░░██░░░░
    0x18, // ░░░██░░░
    0x30, // ░░██░░░░
    0x7F, // ░███████
    0x7F, // ░███████
    0x00, // ░░░░░░░░
          // X   0x58
    0x43, // ░█░░░░██
    0x67, // ░██░░███
    0x3C, // ░░████░░
    0x18, // ░░░██░░░
    0x3C, // ░░████░░
    0x67, // ░██░░███
    0x43, // ░█░░░░██
    0x00, // ░░░░░░░░
          // Y   0x59
    0x07, // ░░░░░███
    0x4F, // ░█░░████
    0x78, // ░████░░░
    0x78, // ░████░░░
    0x4F, // ░█░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // Z   0x5A
    0x47, // ░█░░░███
    0x63, // ░██░░░██
    0x71, // ░███░░░█
    0x59, // ░█░██░░█
    0x4D, // ░█░░██░█
    0x67, // ░██░░███
    0x73, // ░███░░██
    0x00, // ░░░░░░░░
          // [   0x5B
    0x00, // ░░░░░░░░
    0x7F, // ░███████
    0x7F, // ░███████
    0x41, // ░█░░░░░█
    0x41, // ░█░░░░░█
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // \   0x5C
    0x01, // ░░░░░░░█
    0x03, // ░░░░░░██
    0x06, // ░░░░░██░
    0x0C, // ░░░░██░░
    0x18, // ░░░██░░░
    0x30, // ░░██░░░░
    0x60, // ░██░░░░░
    0x00, // ░░░░░░░░
          // ]   0x5D
    0x00, // ░░░░░░░░
    0x41, // ░█░░░░░█
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // ^   0x5E
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x06, // ░░░░░██░
    0x03, // ░░░░░░██
    0x06, // ░░░░░██░
    0x0C, // ░░░░██░░
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
          // _   0x5F
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
          // `   0x60
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x03, // ░░░░░░██
    0x07, // ░░░░░███
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // a   0x61
    0x20, // ░░█░░░░░
    0x74, // ░███░█░░
    0x54, // ░█░█░█░░
    0x54, // ░█░█░█░░
    0x3C, // ░░████░░
    0x78, // ░████░░░
    0x40, // ░█░░░░░░
    0x00, // ░░░░░░░░
          // b   0x62
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x3F, // ░░██████
    0x48, // ░█░░█░░░
    0x48, // ░█░░█░░░
    0x78, // ░████░░░
    0x30, // ░░██░░░░
    0x00, // ░░░░░░░░
          // c   0x63
    0x38, // ░░███░░░
    0x7C, // ░█████░░
    0x44, // ░█░░░█░░
    0x44, // ░█░░░█░░
    0x6C, // ░██░██░░
    0x28, // ░░█░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // d   0x64
    0x30, // ░░██░░░░
    0x78, // ░████░░░
    0x48, // ░█░░█░░░
    0x49, // ░█░░█░░█
    0x3F, // ░░██████
    0x7F, // ░███████
    0x40, // ░█░░░░░░
    0x00, // ░░░░░░░░
          // e   0x65
    0x38, // ░░███░░░
    0x7C, // ░█████░░
    0x54, // ░█░█░█░░
    0x54, // ░█░█░█░░
    0x5C, // ░█░███░░
    0x18, // ░░░██░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // f   0x66
    0x48, // ░█░░█░░░
    0x7E, // ░██████░
    0x7F, // ░███████
    0x49, // ░█░░█░░█
    0x03, // ░░░░░░██
    0x02, // ░░░░░░█░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // g   0x67
    0x98, // █░░██░░░
    0xBC, // █░████░░
    0xA4, // █░█░░█░░
    0xA4, // █░█░░█░░
    0xF8, // █████░░░
    0x7C, // ░█████░░
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
          // h   0x68
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x08, // ░░░░█░░░
    0x04, // ░░░░░█░░
    0x7C, // ░█████░░
    0x78, // ░████░░░
    0x00, // ░░░░░░░░
          // i   0x69
    0x00, // ░░░░░░░░
    0x44, // ░█░░░█░░
    0x7D, // ░█████░█
    0x7D, // ░█████░█
    0x40, // ░█░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // j   0x6A
    0x60, // ░██░░░░░
    0xE0, // ███░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0xFD, // ██████░█
    0x7D, // ░█████░█
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // k   0x6B
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x10, // ░░░█░░░░
    0x38, // ░░███░░░
    0x6C, // ░██░██░░
    0x44, // ░█░░░█░░
    0x00, // ░░░░░░░░
          // l   0x6C
    0x00, // ░░░░░░░░
    0x41, // ░█░░░░░█
    0x7F, // ░███████
    0x7F, // ░███████
    0x40, // ░█░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // m   0x6D
    0x7C, // ░█████░░
    0x7C, // ░█████░░
    0x18, // ░░░██░░░
    0x38, // ░░███░░░
    0x1C, // ░░░███░░
    0x7C, // ░█████░░
    0x78, // ░████░░░
    0x00, // ░░░░░░░░
          // n   0x6E
    0x7C, // ░█████░░
    0x7C, // ░█████░░
    0x04, // ░░░░░█░░
    0x04, // ░░░░░█░░
    0x7C, // ░█████░░
    0x78, // ░████░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // o   0x6F
    0x38, // ░░███░░░
    0x7C, // ░█████░░
    0x44, // ░█░░░█░░
    0x44, // ░█░░░█░░
    0x7C, // ░█████░░
    0x38, // ░░███░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // p   0x70
    0x84, // █░░░░█░░
    0xFC, // ██████░░
    0xF8, // █████░░░
    0xA4, // █░█░░█░░
    0x24, // ░░█░░█░░
    0x3C, // ░░████░░
    0x18, // ░░░██░░░
    0x00, // ░░░░░░░░
          // q   0x71
    0x18, // ░░░██░░░
    0x3C, // ░░████░░
    0x24, // ░░█░░█░░
    0xA4, // █░█░░█░░
    0xF8, // █████░░░
    0xFC, // ██████░░
    0x84, // █░░░░█░░
    0x00, // ░░░░░░░░
          // r   0x72
    0x44, // ░█░░░█░░
    0x7C, // ░█████░░
    0x78, // ░████░░░
    0x4C, // ░█░░██░░
    0x04, // ░░░░░█░░
    0x1C, // ░░░███░░
    0x18, // ░░░██░░░
    0x00, // ░░░░░░░░
          // s   0x73
    0x48, // ░█░░█░░░
    0x5C, // ░█░███░░
    0x54, // ░█░█░█░░
    0x54, // ░█░█░█░░
    0x74, // ░███░█░░
    0x24, // ░░█░░█░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // t   0x74
    0x00, // ░░░░░░░░
    0x04, // ░░░░░█░░
    0x3E, // ░░█████░
    0x7F, // ░███████
    0x44, // ░█░░░█░░
    0x24, // ░░█░░█░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // u   0x75
    0x3C, // ░░████░░
    0x7C, // ░█████░░
    0x40, // ░█░░░░░░
    0x40, // ░█░░░░░░
    0x3C, // ░░████░░
    0x7C, // ░█████░░
    0x40, // ░█░░░░░░
    0x00, // ░░░░░░░░
          // v   0x76
    0x1C, // ░░░███░░
    0x3C, // ░░████░░
    0x60, // ░██░░░░░
    0x60, // ░██░░░░░
    0x3C, // ░░████░░
    0x1C, // ░░░███░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // w   0x77
    0x3C, // ░░████░░
    0x7C, // ░█████░░
    0x70, // ░███░░░░
    0x38, // ░░███░░░
    0x70, // ░███░░░░
    0x7C, // ░█████░░
    0x3C, // ░░████░░
    0x00, // ░░░░░░░░
          // x   0x78
    0x44, // ░█░░░█░░
    0x6C, // ░██░██░░
    0x38, // ░░███░░░
    0x10, // ░░░█░░░░
    0x38, // ░░███░░░
    0x6C, // ░██░██░░
    0x44, // ░█░░░█░░
    0x00, // ░░░░░░░░
          // y   0x79
    0x9C, // █░░███░░
    0xBC, // █░████░░
    0xA0, // █░█░░░░░
    0xA0, // █░█░░░░░
    0xFC, // ██████░░
    0x7C, // ░█████░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // z   0x7A
    0x4C, // ░█░░██░░
    0x64, // ░██░░█░░
    0x74, // ░███░█░░
    0x5C, // ░█░███░░
    0x4C, // ░█░░██░░
    0x64, // ░██░░█░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // {   0x7B
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x3E, // ░░█████░
    0x77, // ░███░███
    0x41, // ░█░░░░░█
    0x41, // ░█░░░░░█
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // |   0x7C
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x77, // ░███░███
    0x77, // ░███░███
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // }   0x7D
    0x41, // ░█░░░░░█
    0x41, // ░█░░░░░█
    0x77, // ░███░███
    0x3E, // ░░█████░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // ~   0x7E
    0x02, // ░░░░░░█░
    0x03, // ░░░░░░██
    0x01, // ░░░░░░░█
    0x03, // ░░░░░░██
    0x02, // ░░░░░░█░
    0x03, // ░░░░░░██
    0x01, // ░░░░░░░█
    0x00, // ░░░░░░░░
};
#endif

#if defined(_SSD1306_FONT_8x16)
const uint8_t font_8x16[] = {
          //     0x20
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // !   0x21
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x38, // ░░███░░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x38, // ░░███░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0D, // ░░░░██░█
    0x0D, // ░░░░██░█
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // "   0x22
    0x00, // ░░░░░░░░
    0x0E, // ░░░░███░
    0x1E, // ░░░████░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x1E, // ░░░████░
    0x0E, // ░░░░███░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // #   0x23
    0x20, // ░░█░░░░░
    0xF8, // █████░░░
    0xF8, // █████░░░
    0x20, // ░░█░░░░░
    0xF8, // █████░░░
    0xF8, // █████░░░
    0x20, // ░░█░░░░░
    0x00, // ░░░░░░░░
    0x02, // ░░░░░░█░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x02, // ░░░░░░█░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x02, // ░░░░░░█░
    0x00, // ░░░░░░░░
          // $   0x24
    0x38, // ░░███░░░
    0x7C, // ░█████░░
    0x44, // ░█░░░█░░
    0x47, // ░█░░░███
    0x47, // ░█░░░███
    0xCC, // ██░░██░░
    0x98, // █░░██░░░
    0x00, // ░░░░░░░░
    0x06, // ░░░░░██░
    0x0C, // ░░░░██░░
    0x08, // ░░░░█░░░
    0x38, // ░░███░░░
    0x38, // ░░███░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // %   0x25
    0x30, // ░░██░░░░
    0x30, // ░░██░░░░
    0x00, // ░░░░░░░░
    0x80, // █░░░░░░░
    0xC0, // ██░░░░░░
    0x60, // ░██░░░░░
    0x30, // ░░██░░░░
    0x00, // ░░░░░░░░
    0x0C, // ░░░░██░░
    0x06, // ░░░░░██░
    0x03, // ░░░░░░██
    0x01, // ░░░░░░░█
    0x00, // ░░░░░░░░
    0x0C, // ░░░░██░░
    0x0C, // ░░░░██░░
    0x00, // ░░░░░░░░
          // &   0x26
    0x80, // █░░░░░░░
    0xD8, // ██░██░░░
    0x7C, // ░█████░░
    0xE4, // ███░░█░░
    0xBC, // █░████░░
    0xD8, // ██░██░░░
    0x40, // ░█░░░░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
          // '   0x27
    0x00, // ░░░░░░░░
    0x10, // ░░░█░░░░
    0x1E, // ░░░████░
    0x0E, // ░░░░███░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // (   0x28
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0xF0, // ████░░░░
    0xF8, // █████░░░
    0x0C, // ░░░░██░░
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x03, // ░░░░░░██
    0x07, // ░░░░░███
    0x0C, // ░░░░██░░
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // )   0x29
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x04, // ░░░░░█░░
    0x0C, // ░░░░██░░
    0xF8, // █████░░░
    0xF0, // ████░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x07, // ░░░░░███
    0x03, // ░░░░░░██
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // *   0x2A
    0x80, // █░░░░░░░
    0xA0, // █░█░░░░░
    0xE0, // ███░░░░░
    0xC0, // ██░░░░░░
    0xC0, // ██░░░░░░
    0xE0, // ███░░░░░
    0xA0, // █░█░░░░░
    0x80, // █░░░░░░░
    0x00, // ░░░░░░░░
    0x02, // ░░░░░░█░
    0x03, // ░░░░░░██
    0x01, // ░░░░░░░█
    0x01, // ░░░░░░░█
    0x03, // ░░░░░░██
    0x02, // ░░░░░░█░
    0x00, // ░░░░░░░░
          // +   0x2B
    0x00, // ░░░░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0xE0, // ███░░░░░
    0xE0, // ███░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x03, // ░░░░░░██
    0x03, // ░░░░░░██
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // ,   0x2C
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x10, // ░░░█░░░░
    0x1E, // ░░░████░
    0x0E, // ░░░░███░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // -   0x2D
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0x80, // █░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // .   0x2E
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0C, // ░░░░██░░
    0x0C, // ░░░░██░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // /   0x2F
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x80, // █░░░░░░░
    0xC0, // ██░░░░░░
    0x60, // ░██░░░░░
    0x30, // ░░██░░░░
    0x00, // ░░░░░░░░
    0x0C, // ░░░░██░░
    0x06, // ░░░░░██░
    0x03, // ░░░░░░██
    0x01, // ░░░░░░░█
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // 0   0x30
    0xF0, // ████░░░░
    0xF8, // █████░░░
    0x0C, // ░░░░██░░
    0xC4, // ██░░░█░░
    0x0C, // ░░░░██░░
    0xF8, // █████░░░
    0xF0, // ████░░░░
    0x00, // ░░░░░░░░
    0x03, // ░░░░░░██
    0x07, // ░░░░░███
    0x0C, // ░░░░██░░
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x07, // ░░░░░███
    0x03, // ░░░░░░██
    0x00, // ░░░░░░░░
          // 1   0x31
    0x00, // ░░░░░░░░
    0x10, // ░░░█░░░░
    0x18, // ░░░██░░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
          // 2   0x32
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x84, // █░░░░█░░
    0xC4, // ██░░░█░░
    0x64, // ░██░░█░░
    0x3C, // ░░████░░
    0x18, // ░░░██░░░
    0x00, // ░░░░░░░░
    0x0E, // ░░░░███░
    0x0F, // ░░░░████
    0x09, // ░░░░█░░█
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x0C, // ░░░░██░░
    0x00, // ░░░░░░░░
          // 3   0x33
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x44, // ░█░░░█░░
    0x44, // ░█░░░█░░
    0x44, // ░█░░░█░░
    0xFC, // ██████░░
    0xB8, // █░███░░░
    0x00, // ░░░░░░░░
    0x04, // ░░░░░█░░
    0x0C, // ░░░░██░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // 4   0x34
    0xC0, // ██░░░░░░
    0xE0, // ███░░░░░
    0xB0, // █░██░░░░
    0x98, // █░░██░░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x80, // █░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
          // 5   0x35
    0x7C, // ░█████░░
    0x7C, // ░█████░░
    0x44, // ░█░░░█░░
    0x44, // ░█░░░█░░
    0x44, // ░█░░░█░░
    0xC4, // ██░░░█░░
    0x84, // █░░░░█░░
    0x00, // ░░░░░░░░
    0x04, // ░░░░░█░░
    0x0C, // ░░░░██░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // 6   0x36
    0xF0, // ████░░░░
    0xF8, // █████░░░
    0x4C, // ░█░░██░░
    0x44, // ░█░░░█░░
    0x44, // ░█░░░█░░
    0xC0, // ██░░░░░░
    0x80, // █░░░░░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // 7   0x37
    0x0C, // ░░░░██░░
    0x0C, // ░░░░██░░
    0x04, // ░░░░░█░░
    0x84, // █░░░░█░░
    0xC4, // ██░░░█░░
    0x7C, // ░█████░░
    0x3C, // ░░████░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // 8   0x38
    0xB8, // █░███░░░
    0xFC, // ██████░░
    0x44, // ░█░░░█░░
    0x44, // ░█░░░█░░
    0x44, // ░█░░░█░░
    0xFC, // ██████░░
    0xB8, // █░███░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // 9   0x39
    0x38, // ░░███░░░
    0x7C, // ░█████░░
    0x44, // ░█░░░█░░
    0x44, // ░█░░░█░░
    0x44, // ░█░░░█░░
    0xFC, // ██████░░
    0xF8, // █████░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x07, // ░░░░░███
    0x03, // ░░░░░░██
    0x00, // ░░░░░░░░
          // :   0x3A
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x30, // ░░██░░░░
    0x30, // ░░██░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x06, // ░░░░░██░
    0x06, // ░░░░░██░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // ;   0x3B
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x30, // ░░██░░░░
    0x30, // ░░██░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0E, // ░░░░███░
    0x06, // ░░░░░██░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // <   0x3C
    0x00, // ░░░░░░░░
    0x80, // █░░░░░░░
    0xC0, // ██░░░░░░
    0x60, // ░██░░░░░
    0x30, // ░░██░░░░
    0x18, // ░░░██░░░
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x01, // ░░░░░░░█
    0x03, // ░░░░░░██
    0x06, // ░░░░░██░
    0x0C, // ░░░░██░░
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
          // =   0x3D
    0x00, // ░░░░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x01, // ░░░░░░░█
    0x01, // ░░░░░░░█
    0x01, // ░░░░░░░█
    0x01, // ░░░░░░░█
    0x01, // ░░░░░░░█
    0x01, // ░░░░░░░█
    0x00, // ░░░░░░░░
          // >   0x3E
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x18, // ░░░██░░░
    0x30, // ░░██░░░░
    0x60, // ░██░░░░░
    0xC0, // ██░░░░░░
    0x80, // █░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x06, // ░░░░░██░
    0x03, // ░░░░░░██
    0x01, // ░░░░░░░█
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // ?   0x3F
    0x18, // ░░░██░░░
    0x1C, // ░░░███░░
    0x04, // ░░░░░█░░
    0xC4, // ██░░░█░░
    0xE4, // ███░░█░░
    0x3C, // ░░████░░
    0x18, // ░░░██░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0D, // ░░░░██░█
    0x0D, // ░░░░██░█
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // @   0x40
    0xF0, // ████░░░░
    0xF8, // █████░░░
    0x08, // ░░░░█░░░
    0xC8, // ██░░█░░░
    0xC8, // ██░░█░░░
    0xF8, // █████░░░
    0xF0, // ████░░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x0B, // ░░░░█░██
    0x0B, // ░░░░█░██
    0x0B, // ░░░░█░██
    0x01, // ░░░░░░░█
    0x00, // ░░░░░░░░
          // A   0x41
    0xE0, // ███░░░░░
    0xF0, // ████░░░░
    0x98, // █░░██░░░
    0x8C, // █░░░██░░
    0x98, // █░░██░░░
    0xF0, // ████░░░░
    0xE0, // ███░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
          // B   0x42
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x44, // ░█░░░█░░
    0x44, // ░█░░░█░░
    0xFC, // ██████░░
    0xB8, // █░███░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // C   0x43
    0xF0, // ████░░░░
    0xF8, // █████░░░
    0x0C, // ░░░░██░░
    0x04, // ░░░░░█░░
    0x04, // ░░░░░█░░
    0x0C, // ░░░░██░░
    0x18, // ░░░██░░░
    0x00, // ░░░░░░░░
    0x03, // ░░░░░░██
    0x07, // ░░░░░███
    0x0C, // ░░░░██░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x06, // ░░░░░██░
    0x00, // ░░░░░░░░
          // D   0x44
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x04, // ░░░░░█░░
    0x0C, // ░░░░██░░
    0xF8, // █████░░░
    0xF0, // ████░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x07, // ░░░░░███
    0x03, // ░░░░░░██
    0x00, // ░░░░░░░░
          // E   0x45
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x44, // ░█░░░█░░
    0xE4, // ███░░█░░
    0x0C, // ░░░░██░░
    0x1C, // ░░░███░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x0E, // ░░░░███░
    0x00, // ░░░░░░░░
          // F   0x46
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x44, // ░█░░░█░░
    0xE4, // ███░░█░░
    0x0C, // ░░░░██░░
    0x1C, // ░░░███░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // G   0x47
    0xF0, // ████░░░░
    0xF8, // █████░░░
    0x0C, // ░░░░██░░
    0x84, // █░░░░█░░
    0x84, // █░░░░█░░
    0x8C, // █░░░██░░
    0x98, // █░░██░░░
    0x00, // ░░░░░░░░
    0x03, // ░░░░░░██
    0x07, // ░░░░░███
    0x0C, // ░░░░██░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
          // H   0x48
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x40, // ░█░░░░░░
    0x40, // ░█░░░░░░
    0x40, // ░█░░░░░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
          // I   0x49
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // J   0x4A
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // K   0x4B
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0xC0, // ██░░░░░░
    0xE0, // ███░░░░░
    0x3C, // ░░████░░
    0x1C, // ░░░███░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
    0x01, // ░░░░░░░█
    0x0F, // ░░░░████
    0x0E, // ░░░░███░
    0x00, // ░░░░░░░░
          // L   0x4C
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x0E, // ░░░░███░
    0x00, // ░░░░░░░░
          // M   0x4D
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x38, // ░░███░░░
    0x70, // ░███░░░░
    0x38, // ░░███░░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
          // N   0x4E
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x38, // ░░███░░░
    0x70, // ░███░░░░
    0xE0, // ███░░░░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
          // O   0x4F
    0xF8, // █████░░░
    0xFC, // ██████░░
    0x04, // ░░░░░█░░
    0x04, // ░░░░░█░░
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xF8, // █████░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // P   0x50
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x44, // ░█░░░█░░
    0x44, // ░█░░░█░░
    0x7C, // ░█████░░
    0x38, // ░░███░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // Q   0x51
    0xF8, // █████░░░
    0xFC, // ██████░░
    0x04, // ░░░░░█░░
    0x04, // ░░░░░█░░
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xF8, // █████░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x0E, // ░░░░███░
    0x3C, // ░░████░░
    0x3F, // ░░██████
    0x27, // ░░█░░███
    0x00, // ░░░░░░░░
          // R   0x52
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x44, // ░█░░░█░░
    0xC4, // ██░░░█░░
    0xFC, // ██████░░
    0x38, // ░░███░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
          // S   0x53
    0x18, // ░░░██░░░
    0x3C, // ░░████░░
    0x64, // ░██░░█░░
    0x44, // ░█░░░█░░
    0xC4, // ██░░░█░░
    0x9C, // █░░███░░
    0x18, // ░░░██░░░
    0x00, // ░░░░░░░░
    0x06, // ░░░░░██░
    0x0E, // ░░░░███░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // T   0x54
    0x00, // ░░░░░░░░
    0x1C, // ░░░███░░
    0x0C, // ░░░░██░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x0C, // ░░░░██░░
    0x1C, // ░░░███░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // U   0x55
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // V   0x56
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x00, // ░░░░░░░░
    0x01, // ░░░░░░░█
    0x03, // ░░░░░░██
    0x06, // ░░░░░██░
    0x0C, // ░░░░██░░
    0x06, // ░░░░░██░
    0x03, // ░░░░░░██
    0x01, // ░░░░░░░█
    0x00, // ░░░░░░░░
          // W   0x57
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x00, // ░░░░░░░░
    0xC0, // ██░░░░░░
    0x00, // ░░░░░░░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x0E, // ░░░░███░
    0x03, // ░░░░░░██
    0x0E, // ░░░░███░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // X   0x58
    0x0C, // ░░░░██░░
    0x3C, // ░░████░░
    0xF0, // ████░░░░
    0xE0, // ███░░░░░
    0xF0, // ████░░░░
    0x3C, // ░░████░░
    0x0C, // ░░░░██░░
    0x00, // ░░░░░░░░
    0x0C, // ░░░░██░░
    0x0F, // ░░░░████
    0x03, // ░░░░░░██
    0x01, // ░░░░░░░█
    0x03, // ░░░░░░██
    0x0F, // ░░░░████
    0x0C, // ░░░░██░░
    0x00, // ░░░░░░░░
          // Y   0x59
    0x00, // ░░░░░░░░
    0x3C, // ░░████░░
    0x7C, // ░█████░░
    0xC0, // ██░░░░░░
    0xC0, // ██░░░░░░
    0x7C, // ░█████░░
    0x3C, // ░░████░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // Z   0x5A
    0x1C, // ░░░███░░
    0x0C, // ░░░░██░░
    0x84, // █░░░░█░░
    0xC4, // ██░░░█░░
    0x64, // ░██░░█░░
    0x3C, // ░░████░░
    0x1C, // ░░░███░░
    0x00, // ░░░░░░░░
    0x0E, // ░░░░███░
    0x0F, // ░░░░████
    0x09, // ░░░░█░░█
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x0E, // ░░░░███░
    0x00, // ░░░░░░░░
          // [   0x5B
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x04, // ░░░░░█░░
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // \   0x5C
    0x38, // ░░███░░░
    0x70, // ░███░░░░
    0xE0, // ███░░░░░
    0xC0, // ██░░░░░░
    0x80, // █░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x01, // ░░░░░░░█
    0x03, // ░░░░░░██
    0x07, // ░░░░░███
    0x0E, // ░░░░███░
    0x00, // ░░░░░░░░
          // ]   0x5D
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x04, // ░░░░░█░░
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // ^   0x5E
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x06, // ░░░░░██░
    0x03, // ░░░░░░██
    0x06, // ░░░░░██░
    0x0C, // ░░░░██░░
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // _   0x5F
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
          // `   0x60
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x03, // ░░░░░░██
    0x07, // ░░░░░███
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // a   0x61
    0x00, // ░░░░░░░░
    0xA0, // █░█░░░░░
    0xA0, // █░█░░░░░
    0xA0, // █░█░░░░░
    0xE0, // ███░░░░░
    0xC0, // ██░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
          // b   0x62
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x20, // ░░█░░░░░
    0x60, // ░██░░░░░
    0xC0, // ██░░░░░░
    0x80, // █░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // c   0x63
    0xC0, // ██░░░░░░
    0xE0, // ███░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x60, // ░██░░░░░
    0x40, // ░█░░░░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
          // d   0x64
    0x80, // █░░░░░░░
    0xC0, // ██░░░░░░
    0x60, // ░██░░░░░
    0x24, // ░░█░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
          // e   0x65
    0xC0, // ██░░░░░░
    0xE0, // ███░░░░░
    0xA0, // █░█░░░░░
    0xA0, // █░█░░░░░
    0xA0, // █░█░░░░░
    0xE0, // ███░░░░░
    0xC0, // ██░░░░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
          // f   0x66
    0x40, // ░█░░░░░░
    0xF8, // █████░░░
    0xFC, // ██████░░
    0x44, // ░█░░░█░░
    0x0C, // ░░░░██░░
    0x18, // ░░░██░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // g   0x67
    0xC0, // ██░░░░░░
    0xE0, // ███░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0xC0, // ██░░░░░░
    0xE0, // ███░░░░░
    0x20, // ░░█░░░░░
    0x00, // ░░░░░░░░
    0x27, // ░░█░░███
    0x6F, // ░██░████
    0x48, // ░█░░█░░░
    0x48, // ░█░░█░░░
    0x7F, // ░███████
    0x3F, // ░░██████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // h   0x68
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x40, // ░█░░░░░░
    0x20, // ░░█░░░░░
    0xE0, // ███░░░░░
    0xC0, // ██░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
          // i   0x69
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x20, // ░░█░░░░░
    0xEC, // ███░██░░
    0xEC, // ███░██░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // j   0x6A
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x20, // ░░█░░░░░
    0xEC, // ███░██░░
    0xEC, // ███░██░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x30, // ░░██░░░░
    0x70, // ░███░░░░
    0x40, // ░█░░░░░░
    0x40, // ░█░░░░░░
    0x7F, // ░███████
    0x3F, // ░░██████
    0x00, // ░░░░░░░░
          // k   0x6B
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x80, // █░░░░░░░
    0xC0, // ██░░░░░░
    0x60, // ░██░░░░░
    0x20, // ░░█░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x01, // ░░░░░░░█
    0x03, // ░░░░░░██
    0x0E, // ░░░░███░
    0x0C, // ░░░░██░░
    0x00, // ░░░░░░░░
          // l   0x6C
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x04, // ░░░░░█░░
    0xFC, // ██████░░
    0xFC, // ██████░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // m   0x6D
    0xE0, // ███░░░░░
    0xE0, // ███░░░░░
    0x60, // ░██░░░░░
    0xC0, // ██░░░░░░
    0x60, // ░██░░░░░
    0xE0, // ███░░░░░
    0xC0, // ██░░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
          // n   0x6E
    0x20, // ░░█░░░░░
    0xE0, // ███░░░░░
    0xC0, // ██░░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0xE0, // ███░░░░░
    0xC0, // ██░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
          // o   0x6F
    0xC0, // ██░░░░░░
    0xE0, // ███░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0xE0, // ███░░░░░
    0xC0, // ██░░░░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // p   0x70
    0x20, // ░░█░░░░░
    0xE0, // ███░░░░░
    0xC0, // ██░░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0xE0, // ███░░░░░
    0xC0, // ██░░░░░░
    0x00, // ░░░░░░░░
    0x40, // ░█░░░░░░
    0x7F, // ░███████
    0x7F, // ░███████
    0x48, // ░█░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // q   0x71
    0xC0, // ██░░░░░░
    0xE0, // ███░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0xC0, // ██░░░░░░
    0xE0, // ███░░░░░
    0x20, // ░░█░░░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x48, // ░█░░█░░░
    0x7F, // ░███████
    0x7F, // ░███████
    0x40, // ░█░░░░░░
    0x00, // ░░░░░░░░
          // r   0x72
    0x20, // ░░█░░░░░
    0xE0, // ███░░░░░
    0xC0, // ██░░░░░░
    0x60, // ░██░░░░░
    0x20, // ░░█░░░░░
    0xE0, // ███░░░░░
    0xC0, // ██░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // s   0x73
    0x40, // ░█░░░░░░
    0xE0, // ███░░░░░
    0xA0, // █░█░░░░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x60, // ░██░░░░░
    0x40, // ░█░░░░░░
    0x00, // ░░░░░░░░
    0x04, // ░░░░░█░░
    0x0C, // ░░░░██░░
    0x09, // ░░░░█░░█
    0x09, // ░░░░█░░█
    0x0B, // ░░░░█░██
    0x0E, // ░░░░███░
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
          // t   0x74
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0xF8, // █████░░░
    0xFC, // ██████░░
    0x20, // ░░█░░░░░
    0x20, // ░░█░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
          // u   0x75
    0xE0, // ███░░░░░
    0xE0, // ███░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0xE0, // ███░░░░░
    0xE0, // ███░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
          // v   0x76
    0x00, // ░░░░░░░░
    0xE0, // ███░░░░░
    0xE0, // ███░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0xE0, // ███░░░░░
    0xE0, // ███░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x03, // ░░░░░░██
    0x07, // ░░░░░███
    0x0C, // ░░░░██░░
    0x0C, // ░░░░██░░
    0x07, // ░░░░░███
    0x03, // ░░░░░░██
    0x00, // ░░░░░░░░
          // w   0x77
    0xE0, // ███░░░░░
    0xE0, // ███░░░░░
    0x00, // ░░░░░░░░
    0x80, // █░░░░░░░
    0x00, // ░░░░░░░░
    0xE0, // ███░░░░░
    0xE0, // ███░░░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x0C, // ░░░░██░░
    0x07, // ░░░░░███
    0x0C, // ░░░░██░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
          // x   0x78
    0x20, // ░░█░░░░░
    0x60, // ░██░░░░░
    0xC0, // ██░░░░░░
    0x80, // █░░░░░░░
    0xC0, // ██░░░░░░
    0x60, // ░██░░░░░
    0x20, // ░░█░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x07, // ░░░░░███
    0x03, // ░░░░░░██
    0x07, // ░░░░░███
    0x0C, // ░░░░██░░
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
          // y   0x79
    0xE0, // ███░░░░░
    0xE0, // ███░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0xE0, // ███░░░░░
    0xE0, // ███░░░░░
    0x00, // ░░░░░░░░
    0x47, // ░█░░░███
    0x4F, // ░█░░████
    0x48, // ░█░░█░░░
    0x48, // ░█░░█░░░
    0x68, // ░██░█░░░
    0x3F, // ░░██████
    0x1F, // ░░░█████
    0x00, // ░░░░░░░░
          // z   0x7A
    0x60, // ░██░░░░░
    0x60, // ░██░░░░░
    0x20, // ░░█░░░░░
    0xA0, // █░█░░░░░
    0xE0, // ███░░░░░
    0x60, // ░██░░░░░
    0x20, // ░░█░░░░░
    0x00, // ░░░░░░░░
    0x0C, // ░░░░██░░
    0x0E, // ░░░░███░
    0x0B, // ░░░░█░██
    0x09, // ░░░░█░░█
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x0C, // ░░░░██░░
    0x00, // ░░░░░░░░
          // {   0x7B
    0x00, // ░░░░░░░░
    0x40, // ░█░░░░░░
    0x40, // ░█░░░░░░
    0xF8, // █████░░░
    0xBC, // █░████░░
    0x04, // ░░░░░█░░
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x07, // ░░░░░███
    0x0F, // ░░░░████
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x00, // ░░░░░░░░
          // |   0x7C
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0xBC, // █░████░░
    0xBC, // █░████░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x0F, // ░░░░████
    0x0F, // ░░░░████
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // }   0x7D
    0x00, // ░░░░░░░░
    0x04, // ░░░░░█░░
    0x04, // ░░░░░█░░
    0xBC, // █░████░░
    0xF8, // █████░░░
    0x40, // ░█░░░░░░
    0x40, // ░█░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x08, // ░░░░█░░░
    0x08, // ░░░░█░░░
    0x0F, // ░░░░████
    0x07, // ░░░░░███
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
          // ~   0x7E
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x04, // ░░░░░█░░
    0x0C, // ░░░░██░░
    0x08, // ░░░░█░░░
    0x0C, // ░░░░██░░
    0x04, // ░░░░░█░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
    0x00, // ░░░░░░░░
};
#endif
