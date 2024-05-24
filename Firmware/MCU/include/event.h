/*

Copyright 2024 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include <stdint.h>

#ifndef event_h
#define event_h

#define EV_TIMER_1MS                      _BV(0)
#define EV_SPI_SLAVE_TRANSACTION_COMPLETE _BV(1)

extern volatile uint8_t event;

void event_setup(void);

#endif