/* Josip Medved <jmedved@jmedved.com> * www.medo64.com * MIT License */

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "i2c_master.h"

#define I2C_BAUDRATE_COUNTER  (_XTAL_FREQ / 4 / _I2C_CLOCK_FREQENCY - 1)

#define I2C_READ  1
#define I2C_WRITE 0
#define I2C_ACK   0
#define I2C_NACK  1


void waitIdle(void) {
    while ((SSPCON2 & 0x1F) | SSPSTATbits.R_nW);  // wait until idle    
}

void resetBus(void) {
    //reset I2C bus
    LATC0 = 0;                                // set clock low
    LATC1 = 0;                                // set data low
    TRISC0 = 1;                               // clock pin configured as output
    TRISC1 = 1;                               // data pin configured as input
    for (unsigned char j=0; j<3; j++) {       // repeat 3 times with alternating data
        for (unsigned char i=0; i<10; i++) {  // 9 cycles + 1 extra
            TRISC0 = 0; __delay_us(50);       // force clock low
            TRISC0 = 1; __delay_us(50);       // release clock high
        }
        TRISC1 = !TRISC1;                     // toggle data line
    }
}

void i2c_master_init(void) {
    SSPCON1 = 0;  SSPCON2 = 0;  SSPSTAT = 0;  // reset all

    resetBus();
    SSPCON1bits.SSPM = 0b1000;                // I2C master mode
    SSPCON1bits.SSPEN = 1;                    // enable I2C master mode
    SSP1STATbits.CKE = 1;                     // slew control enabled, low voltage input (SMBus) enables
    SSP1ADD = I2C_BAUDRATE_COUNTER;           // setup speed

    TRISC0 = 1;                               // clock pin configured as input
    TRISC1 = 1;                               // data pin configured as input}
}

bool i2c_master_startRead(const uint8_t address) {
    i2c_master_start();                                                 // Start
    return i2c_master_writeByte((uint8_t)((address << 1) | I2C_READ));  // load address
}

bool i2c_master_readByte(uint8_t* value) {
    if (PIR2bits.BCL1IF) { return false; }

    SSPCON2bits.RCEN = 1;                       // start receive
    while ( !SSPSTATbits.BF );                  // wait for byte

    *value = SSPBUF;                            // read byte
    if (SSPCON2bits.ACKSTAT) { return false; }  // end prematurely if there's an error

    SSPCON2bits.ACKDT = I2C_ACK;                // ACK for the last byte
    SSPCON2bits.ACKEN = 1;                      // initiate acknowledge sequence
    while (SSPCON2bits.ACKEN);                  // wait for done

    return true;                                // return success
}

bool i2c_master_read(uint8_t* value, const uint8_t count) {
    for (char i=0; i<count; i++) {
        SSPCON2bits.RCEN = 1;                       // start receive
        while ( !SSPSTATbits.BF );                  // wait for byte

        *value = SSPBUF;                             // read byte
        if (SSPCON2bits.ACKSTAT) { return false; }  // end prematurely if there's an error

        value++;

        ACKDT = (i < count-1) ? I2C_ACK : I2C_NACK;  // NACK for last byte
        SSPCON2bits.ACKEN = 1;                       // initiate acknowledge sequence
        while (SSPCON2bits.ACKEN);                   // wait for done
    }
    return true;
}


bool i2c_master_startWrite(const uint8_t address) {
    waitIdle();
    i2c_master_start();                                                  // start operation
    return i2c_master_writeByte((uint8_t)((address << 1) | I2C_WRITE));  // load address
}

bool i2c_master_writeByte(const uint8_t value) {
    waitIdle();
    SSPBUF = value;                                  // set data
    if (SSPCON1bits.WCOL) { return false; }          // fail if there is a collision
    while (SSPSTATbits.BF);                          // wait until write is done
    return SSPCON2bits.ACKSTAT ? false : true;       // return if successful
}


void i2c_master_start(void) {
    SSPCON2bits.SEN = 1;      // initiate Start condition
    while (SSPCON2bits.SEN);  // wait until done
}

void i2c_master_stop(void) {
    SSPCON2bits.PEN = 1;      // initiate Stop condition
    while (SSPCON2bits.PEN);  // wait until done
}

void i2c_master_restart(void) {
    SSPCON2bits.RSEN = 1;      // initiate Repeated Start condition
    while (SSPCON2bits.RSEN);  // wait until done
}
