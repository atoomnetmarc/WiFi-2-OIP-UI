/*

Copyright 2024-2026 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#ifndef message_h
#define message_h

#include <stdint.h>

/**
 * Header that contains basic data about the message being sent.
 *
 * After the header expect messageLength bytes of data.
 */
typedef struct __attribute__((packed)) {
    uint8_t id; // Message beeing send after this header.
    uint8_t length;
} MessageHeader_t;

typedef union {
    MessageHeader_t messageHeader;
    uint8_t array[sizeof(MessageHeader_t)];
} MessageHeader_u;

/**
 * System information
 */
typedef struct __attribute__((packed)) {
    uint32_t buildTime;
} MessageSystemInformation_t;

typedef union {
    MessageSystemInformation_t messageSystemInformationData;
    uint8_t array[sizeof(MessageSystemInformation_t)];
} MessageSystemInformation_u;

/**
 * Set LED state of all 41 LEDs
 */
#define MESSAGE_ID_LEDS 1

enum ledState {
    ledStateOff = 0,
    ledStateOn = 1,
    ledStateFadeOff = 2,
    ledStateFadeOn = 3,
    ledStateFadeSlowOff = 4,
    ledStateFadeSlowOn = 5,

    // When fully on, automatically fade off.
    ledStateFadeBlinkOn = 6,
    // When fully off, automatically fade on.
    ledStateFadeBlinkOff = 7,

    // When fully on, automatically fade off.
    ledStateFadeBlinkSlowOn = 8,
    // When fully off, automatically fade on.
    ledStateFadeBlinkSlowOff = 9,
};

typedef struct __attribute__((packed)) {
    uint8_t red[20];
    uint8_t green[20];
    uint8_t backlight;
} MessageLeds_t;

typedef union {
    MessageLeds_t messageLeds;
    uint8_t array[sizeof(MessageLeds_t)];
} MessageLeds_u;

/**
 * Toggle LCD reset
 */
#define MESSAGE_ID_LCD_RESET 2
// This message has no payload.

#endif