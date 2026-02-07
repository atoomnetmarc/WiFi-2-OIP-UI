/*

Copyright 2024-2026 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include <Arduino.h>
#include <SPI.h>
#include <Ticker.h>
#include <U8g2lib.h>

#include <AvrIspProgrammer.h>

#include "instruction.h"
#include "message.h"
#include "spi_config.h"

#include "time.h"

#include "../MCU/include/node_firmware.h"
#include "../MCU/include/node_firmware_build_time.h"

#include "config.h"
#include "display.h"
#include "main.h"
#include "mqtt.h"
#include "wific.h"

Ticker tickerReadSystemInformation;
Ticker tickerNodeDoSomething;

#define SPI_MODE_NETWORK 0
#define SPI_MODE_ISP     1
volatile uint8_t spiMode = SPI_MODE_NETWORK;

void ReadSystemInformation() {
    if (spiMode != SPI_MODE_NETWORK) {
        return;
    }

    SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE0));
    digitalWrite(M_CS, LOW);

    MessageSystemInformation_u si = {};
    SPI.transfer(INSTRUCTION_SYSTEMINFO_READ);
    for (uint8_t i = 0; i < sizeof(si.array); i++) {
        si.array[i] = SPI.transfer(0xFF);
    }

    digitalWrite(M_CS, HIGH);
    SPI.endTransaction();

    Serial.print(F("Node firmware build time: "));
    Serial.println(si.messageSystemInformationData.buildTime);

    // Must the node be flashed?
    if (si.messageSystemInformationData.buildTime != NODE_FIRMWARE_BUILD_TIME) {
        Serial.println(F("Node should be programmed, buildTime of firmware is different."));
        spiMode = SPI_MODE_ISP;
        SPI.end();
        digitalWrite(LED_BUILTIN, HIGH);
        uint8_t result = programNode();
        digitalWrite(LED_BUILTIN, LOW);
        SPI.begin(M_SCK, M_MISO, M_MOSI, M_CS);
        spiMode = SPI_MODE_NETWORK;

        if (result == 0) {
            Serial.println("Programming of node succeeded.");
        } else {
            Serial.println("Programming of node failed.");
        }
    }
    tickerReadSystemInformation.detach();

    // At this point the node is probably ready to process commands.
}

uint8_t programNode() {
    if (spiMode != SPI_MODE_ISP) {
        Serial.println(F("Refusing to program node. spiMode is not SPI_MODE_ISP."));
        return 0;
    }

    if (AvrIspProgrammerEnableSerialProgramming() != 1) {
        Serial.println(F("Unable to enter AVR into programming mode."));
        return 1;
    }

    uint8_t signature[3];
    AvrIspProgrammerReadSignature(signature);

    if (signature[0] != node_signature[0] ||
        signature[1] != node_signature[1] ||
        signature[2] != node_signature[2]) {
        Serial.println(F("Signature of AVR not as expected."));
        return 2;
    }

    FuseBits_t fusebits;
    AvrIspProgrammerReadFuseBits(&fusebits);

    uint8_t erased = 0;
    FuseBits_t targetfusebits = {node_fuse[0], node_fuse[1], node_fuse[2]};
    if (fusebits.low != targetfusebits.low ||
        fusebits.high != targetfusebits.high ||
        fusebits.extended != targetfusebits.extended) {
        AvrIspProgrammerChipErase();
        erased = 1;
        AvrIspProgrammerWriteFuseBits(&targetfusebits);
        Serial.println(F("Fuses of AVR programmed."));
    }

    uint8_t flashcontentsequal = 1;
    for (size_t i = 0; i < sizeof(node_flash); i++) {
        uint8_t currentFlash = AvrIspProgrammerReadProgramMemory(i);

        if (node_flash[i] != currentFlash) {
            flashcontentsequal = 0;
            break;
        }
    }

    if (!flashcontentsequal) {
        Serial.println(F("Programming flash."));
        if (!erased) {
            AvrIspProgrammerChipErase();
        }
        uint8_t pagesizeBytes = 32 * 2; // ATmega48 page size in bytes.
        uint8_t pageByteCounter = 0;
        size_t j = 0;
        for (size_t i = 0; i < sizeof(node_flash); i++) {
            AvrIspProgrammerLoadProgramMemoryPage(pageByteCounter, node_flash[i]);

            if (++pageByteCounter == pagesizeBytes ||
                i == sizeof(node_flash) - 1) {
                AvrIspProgrammerWriteMemoryPage(j);
                pageByteCounter = 0;
                j = i + 1; // Save start address of flash to program page for later.
            }
        }
    } else {
        Serial.println(F("Flash not programmed. Flash is the same as the new firmware."));
    }
    AvrIspProgrammerDisableSerialProgramming();

    Serial.println(F("Programming done."));
    return 0;
}

void NodeDoSomething() {
    Serial.println(F("NodeDoSomething()"));
    if (spiMode != SPI_MODE_NETWORK) {
        return;
    }

    static MessageLeds_u ml = {};

    ml.messageLeds.backlight = ledStateOn;

    ml.messageLeds.red[0] = ledStateOff;
    ml.messageLeds.red[1] = ledStateOn;
    ml.messageLeds.red[2] = ledStateFadeOff;
    ml.messageLeds.red[3] = ledStateFadeOn;
    ml.messageLeds.red[4] = ledStateFadeSlowOff;
    ml.messageLeds.red[5] = ledStateFadeSlowOn;
    ml.messageLeds.red[6] = ledStateFadeBlinkOn;
    ml.messageLeds.red[7] = ledStateFadeBlinkOff;
    ml.messageLeds.red[8] = ledStateFadeBlinkSlowOn;
    ml.messageLeds.red[9] = ledStateFadeBlinkSlowOff;

    if (ml.messageLeds.green[0] == ledStateFadeOn) {
        Serial.println(F("Off"));
        ml.messageLeds.green[0] = ledStateOff;
    } else {
        Serial.println(F("On"));
        ml.messageLeds.green[0] = ledStateFadeOn;
    }

    MessageHeader_u mh = {};
    mh.messageHeader.id = MESSAGE_ID_LEDS;
    mh.messageHeader.length = sizeof(ml.array);

    SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE0));
    digitalWrite(M_CS, LOW);

    SPI.transfer(INSTRUCTION_MESSAGE_WRITE);
    for (uint8_t i = 0; i < sizeof(mh.array); i++) {
        SPI.transfer(mh.array[i]);
    }
    for (uint8_t i = 0; i < sizeof(ml.array); i++) {
        SPI.transfer(ml.array[i]);
    }

    digitalWrite(M_CS, HIGH);
    SPI.endTransaction();
}

/**
 * @brief Arduino setup function.
 *
 * This function is called once at the beginning of the program. It initializes the display,
 * connects to WiFi, initializes the MQTT client, and subscribes to relevant topics.
 */
void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    setCpuFrequencyMhz(80);
    Serial.begin(115200);

    // Wait a while for USB serial connection.
    /*
    uint32_t counter = 0;
    while (!Serial && counter++ < 100) {
        delay(100);
    }
    */

    Serial.println(F("Booting"));

    configTime(0, 0, NTP_SERVER);

    digitalWrite(M_RESET, HIGH);
    pinMode(M_RESET, OUTPUT);
    pinMode(M_INT, PULLUP);
    pinMode(M_CS, OUTPUT);
    digitalWrite(M_CS, HIGH);

    SPI.begin(M_SCK, M_MISO, M_MOSI);

    tickerReadSystemInformation.attach_ms(100, ReadSystemInformation);
    tickerNodeDoSomething.attach_ms(1000, NodeDoSomething);

    WificSetup();
    MqttSetup();
    DisplaySetup();
    RequestDrawDisplay();

    digitalWrite(LED_BUILTIN, LOW);
    Serial.print(F("My hostname: "));
    Serial.println(hostname);
}

/**
 * @brief Arduino loop function.
 *
 * This function is called repeatedly in a loop. It updates the MQTT client and reconnects
 * if the connection is lost.
 */
void loop() {
    DisplayLoop();
    MqttLoop();
    WiFicLoop();
}