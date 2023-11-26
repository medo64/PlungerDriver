#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "settings.h"


#define _SETTINGS_FLASH_RAW { \
                              ' ',  ' ',  ' ',  ' ',  'P',  'l',  'u',  'n',  'g',  'e',  'r',  ' ',  ' ',  ' ',  ' ',  ' ',  \
                              ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  'D',  'r',  'i',  'v',  'e',  'r',  ' ',  ' ',  ' ',  ' ',  \
                              0x00,  10,   20,   30,   50,  100,   10,   10,   10,   10,   10,    5,   10,   15,   25,   50,  \
                            }  // reserving space because erase block is block 32-word (32-bytes as only low bytes are used)
#define _SETTINGS_FLASH_LOCATION 0x1F80
const uint8_t _SETTINGS_PROGRAM[] __at(_SETTINGS_FLASH_LOCATION) = _SETTINGS_FLASH_RAW;

typedef struct {
    uint8_t GreetingLine1[16];
    uint8_t GreetingLine2[16];
    uint8_t Flags;  // 1:DisplayFlipped
    uint8_t Push[5];
    uint8_t Hold[5];
    uint8_t Pull[5];
} SettingsRecord;

SettingsRecord Settings;


void settings_init(void) {
    uint8_t* settingsPtr = (uint8_t*)&Settings;
    for (uint8_t i = 0; i < sizeof(Settings); i++) {
        *settingsPtr = _SETTINGS_PROGRAM[i];
        settingsPtr++;
    }
}

void settings_save(void) {
    bool hadInterruptsEnabled = (INTCONbits.GIE != 0);  // save if interrupts enabled
    INTCONbits.GIE = 0;  // disable interrupts
    PMCON1bits.WREN = 1;  // enable writes

    uint16_t address = _SETTINGS_FLASH_LOCATION;
    uint8_t* settingsPtr = (uint8_t*)&Settings;

    for (uint8_t i = 0; i < sizeof(Settings); i += 32) {
        // erase
        PMADR = address;         // set location
        PMCON1bits.CFGS = 0;     // program space
        PMCON1bits.FREE = 1;     // erase
        PMCON2 = 0x55;           // unlock
        PMCON2 = 0xAA;           // unlock
        PMCON1bits.WR = 1;       // begin erase
        asm("NOP"); asm("NOP");  // forced

        // write
        for (uint8_t j = 0; j < 32; j++) {
            bool latched = (j == 31) ? 0 : 1;  // latch load is done for all except last
            PMADR = address;            // set location
            PMDATH = 0x3F;              // same as when erased
            PMDATL = *settingsPtr;      // load data
            PMCON1bits.CFGS = 0;        // program space
            PMCON1bits.LWLO = latched;  // load write latches
            PMCON2 = 0x55;              // unlock
            PMCON2 = 0xAA;              // unlock
            PMCON1bits.WR = 1;          // begin write
            asm("NOP"); asm("NOP");     // forced
            address++;                  // move write address
            settingsPtr++;              // move data pointer
        }
    }

    PMCON1bits.WREN = 0;  // disable writes
    if (hadInterruptsEnabled) { INTCONbits.GIE = 1; }  // restore interrupts
}


char* settings_getGreetingLine1(void) {
    return (char*)Settings.GreetingLine1;
}

char* settings_getGreetingLine2(void) {
    return (char*)Settings.GreetingLine2;
}


bool settings_getIsDisplayFlipped(void) {
    return (Settings.Flags & 0x80) != 0;
}

void settings_setIsDisplayFlipped(bool value) {
    if (value) {
        Settings.Flags |= 0x80;
    } else {
        Settings.Flags &= ~0x80;
    }
}

bool settings_getIsDriveFlipped(void) {
    return (Settings.Flags & 0x40) != 0;
}

void settings_setIsDriveFlipped(bool value) {
    if (value) {
        Settings.Flags |= 0x40;
    } else {
        Settings.Flags &= ~0x40;
    }
}


uint8_t settings_getPush(uint8_t index) {
    return Settings.Push[index];
}

void settings_setPush(uint8_t index, uint8_t value) {  // 0-250 ms
    if (value > 250) { value = 250; }
    Settings.Push[index] = value;
}


uint8_t settings_getHold(uint8_t index) {
    return Settings.Hold[index];
}

void settings_setHold(uint8_t index, uint8_t value) {  // 0-250 ms
    if (value > 250) { value = 250; }
    Settings.Hold[index] = value;
}


uint8_t settings_getPull(uint8_t index) {
    return Settings.Pull[index];
}

void settings_setPull(uint8_t index, uint8_t value) {  // 0-250 ms
    if (value > 250) { value = 250; }
    Settings.Pull[index] = value;
}
