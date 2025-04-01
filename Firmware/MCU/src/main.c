/*

Copyright 2024-2025 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include <stdio.h>
#include <string.h>

#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>

#include <util/delay.h>

#include "event.h"
#include "fuses.h"
#include "instruction.h"
#include "io.h"
#include "message.h"
#include "node_firmware_build_time.h"
#include "timer.h"

// Test.
uint16_t EEMEM dummy = 0xABCD;

volatile MessageSystemInformation_u systemInfo = {};

// Define buffer size as header + largest message.
#define BUFFER_SIZE 1 + sizeof(MessageHeader_u) + sizeof(MessageLeds_u)

volatile uint8_t spiSlaveReceiveBuffer[BUFFER_SIZE];
volatile uint8_t spiSlaveReceiveBufferPointer = 0;

volatile uint8_t *spiSlaveTransmitData;
volatile uint8_t spiSlaveTransmitLength = 0;

void SPI_init_slave() {
    // Enable Pin Change Interrupt on falling edge of SS (PB2)
    PCICR |= _BV(PCIE0);   // Enable Pin Change Interrupt 0
    PCMSK0 |= _BV(PCINT2); // Enable interrupt on PB2 (SS)

    // Set SS (PB2) as input with pull-up resistor
    DDRB &= ~_BV(PB2);
    PORTB |= _BV(PB2);

    // Set MISO (PB4) as output
    DDRB |= _BV(PB4);

    // Enable SPI and SPI interrupt
    SPCR = _BV(SPE) | _BV(SPIE);
}

ISR(SPI_STC_vect) {
    // Save incoming data first.
    uint8_t receivedData = SPDR;

    // If this is the first byte since SS went low, then this is the instruction byte.
    if (spiSlaveReceiveBufferPointer == 0) {
        if (receivedData == INSTRUCTION_SYSTEMINFO_READ) {
            spiSlaveTransmitData = systemInfo.array;
            spiSlaveTransmitLength = sizeof(systemInfo.array);
        }
    }

    // Are we expected to send something?
    if (spiSlaveTransmitLength > 0) {
        SPDR = *spiSlaveTransmitData;

        spiSlaveTransmitData++;
        spiSlaveTransmitLength--;
    } else {
        SPDR = 0xFF;
    }

    if (spiSlaveReceiveBufferPointer < BUFFER_SIZE) {
        spiSlaveReceiveBuffer[spiSlaveReceiveBufferPointer] = receivedData;
        spiSlaveReceiveBufferPointer++;
    }
}

ISR(PCINT0_vect) {
    if ((PINB & _BV(PB2)) > 0) {
        event |= EV_SPI_SLAVE_TRANSACTION_COMPLETE;
        spiSlaveTransmitLength = 0;
    } else {
        spiSlaveReceiveBufferPointer = 0;
    }
}

void setup(void) {
    wdt_enable(WDTO_8S);

    io_setup();
    event_setup();
    timer_setup();

    SPI_init_slave();

    sei();

    systemInfo.messageSystemInformationData.buildTime = NODE_FIRMWARE_BUILD_TIME;

    leds[40] = ledStateFadeBlinkSlowOn;
}

void loop(void) {
    wdt_reset();

    while (event == 0) {
    }

    if (event & EV_TIMER_1MS) {
        event &= ~EV_TIMER_1MS;

        static uint16_t ledGreenTimer = 0;
        static uint8_t ledGreenCounter = 0;
        ledGreenTimer++;
        if (ledGreenTimer == 109) {
            ledGreenTimer = 0;
            for (uint8_t i = 0; i < 20; i++) {
                leds[i + 20] = ledStateFadeOff;
            }
            leds[ledGreenCounter + 20] = ledStateOn;
            if (++ledGreenCounter == 20) {
                ledGreenCounter = 0;
            }
        }

        static uint16_t ledRedTimer = 0;
        static int8_t ledRedCounter = 19;
        ledRedTimer++;
        if (ledRedTimer == 503) {
            ledRedTimer = 0;
            for (uint8_t i = 0; i < 20; i++) {
                leds[i] = ledStateFadeSlowOff;
            }
            leds[ledRedCounter] = ledStateFadeOn;
            if (--ledRedCounter < 0) {
                ledRedCounter = 19;
            }
        }

        // Calculate new PWM target values of LEDS for the faster effects.
        static uint16_t updatePWMValuesCounterFast = 0;
        updatePWMValuesCounterFast++;
        if (updatePWMValuesCounterFast == 500 / LED_PWM_MAX) {
            updatePWMValuesCounterFast = 0;

            for (uint8_t i = 0; i < sizeof(leds); i++) {
                if (leds[i] == ledStateOff) {
                    ledsPWM[i] = 0;
                } else if (leds[i] == ledStateOn) {
                    ledsPWM[i] = LED_PWM_MAX;
                } else if (leds[i] == ledStateFadeOff && ledsPWM[i] > 0) {
                    ledsPWM[i]--;
                } else if (leds[i] == ledStateFadeOn && ledsPWM[i] < LED_PWM_MAX) {
                    ledsPWM[i]++;
                } else if (leds[i] == ledStateFadeBlinkOn && ledsPWM[i] < LED_PWM_MAX) {
                    ledsPWM[i]++;
                } else if (leds[i] == ledStateFadeBlinkOn && ledsPWM[i] == LED_PWM_MAX) {
                    leds[i] = ledStateFadeBlinkOff;
                } else if (leds[i] == ledStateFadeBlinkOff && ledsPWM[i] > 0) {
                    ledsPWM[i]--;
                } else if (leds[i] == ledStateFadeBlinkOff && ledsPWM[i] == 0) {
                    leds[i] = ledStateFadeBlinkOn;
                }
            }
        }

        // Calculate new PWM target values of LEDS for the slower effects.
        static uint16_t updatePWMValuesCounterSlow = 0;
        updatePWMValuesCounterSlow++;
        if (updatePWMValuesCounterSlow == 5000 / LED_PWM_MAX) {
            updatePWMValuesCounterSlow = 0;

            for (uint8_t i = 0; i < sizeof(leds); i++) {
                if (leds[i] == ledStateFadeBlinkSlowOn && ledsPWM[i] < LED_PWM_MAX) {
                    ledsPWM[i]++;
                } else if (leds[i] == ledStateFadeBlinkSlowOn && ledsPWM[i] == LED_PWM_MAX) {
                    leds[i] = ledStateFadeBlinkSlowOff;
                } else if (leds[i] == ledStateFadeBlinkSlowOff && ledsPWM[i] > 0) {
                    ledsPWM[i]--;
                } else if (leds[i] == ledStateFadeBlinkSlowOff && ledsPWM[i] == 0) {
                    leds[i] = ledStateFadeBlinkSlowOn;
                } else if (leds[i] == ledStateFadeSlowOff && ledsPWM[i] > 0) {
                    ledsPWM[i]--;
                } else if (leds[i] == ledStateFadeSlowOn && ledsPWM[i] < LED_PWM_MAX) {
                    ledsPWM[i]++;
                }
            }
        }
    }

    if (event & EV_SPI_SLAVE_TRANSACTION_COMPLETE) {
        event &= ~EV_SPI_SLAVE_TRANSACTION_COMPLETE;

        uint8_t offset = 0;
        uint8_t instruction = spiSlaveReceiveBuffer[offset];
        offset += sizeof(instruction);
        if (instruction == INSTRUCTION_MESSAGE_WRITE) {
            MessageHeader_u *mu = (MessageHeader_u *)&spiSlaveReceiveBuffer[offset];
            offset += sizeof(mu->array);

            if (mu->messageHeader.id == MESSAGE_ID_LEDS) {
                MessageLeds_u *ml = (MessageLeds_u *)&spiSlaveReceiveBuffer[offset];
                offset += sizeof(ml->array);

                for (uint8_t i = 0; i < sizeof(ml->array); i++) {
                    leds[i] = ml->array[i];
                }
            } else if (mu->messageHeader.id == MESSAGE_ID_LCD_RESET) {
                lcdReset = 0;
            }
        }
    }
}

__attribute__((OS_main)) int main(void) {
    setup();

    for (;;) {
        loop();
    }
}