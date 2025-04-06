/*

Copyright 2024-2025 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include <avr/io.h>

#include "io.h"
#include "message.h"

uint8_t leds[41];
volatile uint8_t ledsPWM[41];

uint8_t ledColumnCounter = 1;
uint8_t ledPWMCounter = 0;
uint8_t lcdReset = 1;

void io_setup(void) {
    // Set SPKR_PWM and SPKR_EN low.
    SPKR_PORT &= ~_BV(SPKR_EN) & ~_BV(SPKR_PWM);
    SPKR_DDR |= _BV(SPKR_EN) | _BV(SPKR_PWM);

    MATRIX_FF_CLK_DDR |= _BV(MATRIX_FF_CLK_COLUMN) | _BV(MATRIX_FF_CLK_ROW);

    SPI_SLAVE_INT_PORT |= _BV(SPI_SLAVE_INT);
    SPI_SLAVE_INT_DDR |= _BV(SPI_SLAVE_INT);

    // Matrix flip flops data byte.
    DDRC |= _BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3);
    DDRD |= _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7);

    // Debug pin
    debug_pin_low();
    DEBUG_PIN_DDR |= _BV(DEBUG_PIN);
}
