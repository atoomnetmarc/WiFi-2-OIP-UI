/*

Copyright 2024 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include "message.h"

#ifndef io_h
#define io_h

#define SPKR_EN   PB0
#define SPKR_PWM  PB1
#define SPKR_DDR  DDRB
#define SPKR_PORT PORTB

#define MATRIX_FF_CLK_DDR    DDRD
#define MATRIX_FF_CLK_PORT   PORTD
#define MATRIX_FF_CLK_COLUMN PD0
#define MATRIX_FF_CLK_ROW    PD2

#define SPI_SLAVE_INT      PC4
#define SPI_SLAVE_INT_DDR  DDRC
#define SPI_SLAVE_INT_PORT PORTC

#define DEBUG_PIN      PD1
#define DEBUG_PIN_DDR  DDRD
#define DEBUG_PIN_PORT PORTD

/**
 * Assign some ledState enum value.
 *
 * First 20 is red.
 * Second 20 is green.
 * Last one is LCD backlight.
 */
extern uint8_t leds[41];

/**
 * The current leds PWM value;
 */
extern volatile uint8_t ledsPWM[41];
#define LED_PWM_MAX 100

extern uint8_t ledColumnCounter;
extern uint8_t ledPWMCounter;

/**
 * Set to 0 to pulse LOW for 1ms and reset the LCD.
 */
extern uint8_t lcdReset;

void io_setup(void);

inline void spi_slave_int_low(void) {
    SPI_SLAVE_INT_PORT &= ~_BV(SPI_SLAVE_INT);
}

inline void spi_slave_int_high(void) {
    SPI_SLAVE_INT_PORT |= _BV(SPI_SLAVE_INT);
}

inline void matrix_ff_clk_column(void) {
    MATRIX_FF_CLK_PORT |= _BV(MATRIX_FF_CLK_COLUMN);
    MATRIX_FF_CLK_PORT &= ~_BV(MATRIX_FF_CLK_COLUMN);
}

inline void matrix_ff_clk_row(void) {
    MATRIX_FF_CLK_PORT |= _BV(MATRIX_FF_CLK_ROW);
    MATRIX_FF_CLK_PORT &= ~_BV(MATRIX_FF_CLK_ROW);
}

inline void matrix_ff_data(uint8_t byte) {
    PORTC &= 0xF0;
    PORTD &= 0x0F;

    PORTC |= byte & 0x0F;
    PORTD |= byte & 0xF0;
}

inline void matrix_ff_update() {
    matrix_ff_data(0x00);
    matrix_ff_clk_row();

    uint8_t ledColumnSelector = 1 << ledColumnCounter;
    ledColumnSelector = ~ledColumnSelector & 0b00011111;

    matrix_ff_data(ledColumnSelector | ((ledsPWM[40] >= ledPWMCounter) << 5) | (lcdReset << 6));
    matrix_ff_clk_column();

    uint8_t ledColumnOffset = ledColumnCounter * 4;
    matrix_ff_data((ledsPWM[ledColumnOffset + 0] >= ledPWMCounter) << 0 |
                   (ledsPWM[ledColumnOffset + 1] >= ledPWMCounter) << 1 |
                   (ledsPWM[ledColumnOffset + 2] >= ledPWMCounter) << 2 |
                   (ledsPWM[ledColumnOffset + 3] >= ledPWMCounter) << 3 |
                   (ledsPWM[ledColumnOffset + 0 + 20] >= ledPWMCounter) << 4 |
                   (ledsPWM[ledColumnOffset + 1 + 20] >= ledPWMCounter) << 5 |
                   (ledsPWM[ledColumnOffset + 2 + 20] >= ledPWMCounter) << 6 |
                   (ledsPWM[ledColumnOffset + 3 + 20] >= ledPWMCounter) << 7);

    matrix_ff_clk_row();

    ledColumnCounter++;
    if (ledColumnCounter == 5) {
        ledColumnCounter = 0;
    }

    ledPWMCounter++;
    if (ledPWMCounter > LED_PWM_MAX) {
        ledPWMCounter = 1;
    }

    lcdReset = 1;
}

inline void debug_pin_low(void) {
    DEBUG_PIN_PORT &= ~_BV(DEBUG_PIN);
}

inline void debug_pin_high(void) {
    DEBUG_PIN_PORT |= _BV(DEBUG_PIN);
}

#endif