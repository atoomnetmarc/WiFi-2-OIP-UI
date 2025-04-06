/*

Copyright 2024-2025 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include <Arduino.h>

#include "config.h"

#ifndef AvrIspProgrammerImplementationSpecific_H
#define AvrIspProgrammerImplementationSpecific_H

/**
 * Initializes all the ISP I/O pins.
 */
inline void AvrIspProgrammerIoInit(void) {
    pinMode(M_RESET, OUTPUT);
    pinMode(M_SCK, OUTPUT);
    pinMode(M_MOSI, OUTPUT);
    pinMode(M_MISO, INPUT);
}

/**
 * Set reset high.
 */
inline void AvrIspProgrammerIoResetHigh(void) {
    digitalWrite(M_RESET, HIGH);
}

/**
 * Set reset low.
 */
inline void AvrIspProgrammerIoResetLow(void) {
    digitalWrite(M_RESET, LOW);
}

/**
 * Set SCK high.
 */
inline void AvrIspProgrammerIoSckHigh(void) {
    digitalWrite(M_SCK, HIGH);
}

/**
 * Set SCK low.
 */
inline void AvrIspProgrammerIoSckLow(void) {
    digitalWrite(M_SCK, LOW);
}

/**
 * Set MOSI high.
 */
inline void AvrIspProgrammerIoMosiHigh(void) {
    digitalWrite(M_MOSI, HIGH);
}

/**
 * Set MOSI low.
 */
inline void AvrIspProgrammerIoMosiLow(void) {
    digitalWrite(M_MOSI, LOW);
}

/**
 * Read MISO
 */
inline uint8_t AvrIspProgrammerIoMiso(void) {
    return digitalRead(M_MISO);
}

/**
 * Delay certain amount of micro seconds.
 */
inline void AvrIspProgrammerDelayUs(double us) {
    delayMicroseconds(us);
}

#endif
