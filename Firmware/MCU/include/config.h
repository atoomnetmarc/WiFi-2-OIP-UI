/*

Copyright 2024-2026 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include <avr/io.h>

#ifndef config_h
#define config_h

#define M_DDR   DDRD
#define M_PIN   PIND
#define M_PORT  PORTD
#define M_RESET PD3
#define M_SCK   PD4
#define M_MOSI  PD1
#define M_MISO  PD0

#endif