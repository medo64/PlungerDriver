#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "app.h"

void settings_init(void);
void settings_save(void);

char* settings_getGreetingLine1(void);
char* settings_getGreetingLine2(void);

bool settings_getIsDisplayFlipped(void);
void settings_setIsDisplayFlipped(bool value);

uint8_t settings_getPush(uint8_t index);
void settings_setPush(uint8_t index, uint8_t value);

uint8_t settings_getHold(uint8_t index);
void settings_setHold(uint8_t index, uint8_t value);

uint8_t settings_getPull(uint8_t index);
void settings_setPull(uint8_t index, uint8_t value);
