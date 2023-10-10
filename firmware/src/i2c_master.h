/* Josip Medved <jmedved@jmedved.com> * www.medo64.com * MIT License */

/**
 * I2C master communication.
 * 
 * Defines:
 *   _XTAL_FREQ <N>: frequency in Hz
 *   _I2C_CLOCK_FREQENCY <N>: I2C bus frequency in Hz (default 100000)
 */
// 2023-10-09: Refactoring

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "app.h"

#if !defined(_16F1454) && !defined(_16F1455)
#error Unsupported device
#endif

#if !defined(_XTAL_FREQ)
#error Must define _XTAL_FREQ
#endif

#if !defined(_I2C_CLOCK_FREQENCY)
#define _I2C_CLOCK_FREQENCY  100000
#endif


/** Initializes I2C as a master. */
void i2c_master_init();


/** Starts a read operation. */
bool i2c_master_startRead(const uint8_t address);

/** Reads single byte. */
bool i2c_master_readByte(uint8_t* value);

/** Reads multiple bytes. */
bool i2c_master_read(uint8_t* value, const uint8_t count);


/** Starts a writing operation. */
bool i2c_master_startWrite(const uint8_t address);

/** Writes single byte. */
bool i2c_master_writeByte(const uint8_t value);


/** Sends a start command. */
void i2c_master_start(void);

/** Sends a stop command. */
void i2c_master_stop(void);

/** Sends a restart command. */
void i2c_master_restart(void);
