/*

Copyright 2024-2026 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include <avr/interrupt.h>
#include <avr/power.h>

#include "event.h"
#include "io.h"
#include "timer.h"

ISR(TIMER0_OVF_vect) {
    debug_pin_high();
    uint8_t clkdiv = 64;
    uint16_t timerHz = 10 * 1000;

    TCNT0 = 256 - (F_CPU / clkdiv / timerHz);

    matrix_ff_update();

    static uint8_t mscounter = 0;
    mscounter++;
    if (mscounter == timerHz / 1000) {
        mscounter = 0;
        // Signal that 1ms has passed.
        event |= EV_TIMER_1MS;
    }
    debug_pin_low();
}

void timer_setup(void) {
    // Configure timer0
    TCCR0B |= _BV(CS00) | _BV(CS01); // clk/64
    TIMSK0 |= _BV(TOIE0);
}
