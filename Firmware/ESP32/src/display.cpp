/*

Copyright 2024-2026 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

#include "config.h"
#include "display.h"
#include "main.h"
#include "wific.h"

U8G2_ST7565_ERC12864_ALT_F_4W_HW_SPI u8g2(U8G2_R0, LCD_CS, LCD_DC);

volatile uint8_t requestDrawDisplay = 0;

void RequestDrawDisplay() {
    requestDrawDisplay = 1;
}

void DrawDisplay() {
    u8g2.clearBuffer();
    u8g2.setFontDirection(0);

    u8g2.setFont(u8g2_font_profont11_tf);
    u8g2.setCursor(0, 10);
    u8g2.print("hostname:");

    u8g2.setFont(u8g2_font_profont11_tf);
    u8g2.setCursor(0, 20);
    u8g2.print(hostname);

    u8g2.setFont(u8g2_font_profont11_tf);
    u8g2.setCursor(0, 40);
    u8g2.print("https://github.com");
    u8g2.setCursor(0, 50);
    u8g2.print("/atoomnetmarc");
    u8g2.setCursor(0, 60);
    u8g2.print("/WiFi-2-OIP-UI");

    u8g2.sendBuffer();
}

void DisplaySetup() {
    u8g2.begin();
    u8g2.enableUTF8Print();
}

void DisplayLoop() {
    if (requestDrawDisplay) {
        requestDrawDisplay = 0;
        DrawDisplay();
    }
}

void DrawBitmap(const uint8_t *bitmap) {
    u8g2.clearBuffer();
    u8g2.drawXBMP(0, 0, 128, 64, bitmap);
    u8g2.sendBuffer();
}
