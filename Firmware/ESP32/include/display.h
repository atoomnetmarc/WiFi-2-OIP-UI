/*

Copyright 2024-2026 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#ifndef display_h
#define display_h

#include <U8g2lib.h>

extern U8G2_ST7565_ERC12864_ALT_F_4W_HW_SPI u8g2;

void DisplaySetup(void);
void DisplayLoop(void);
void RequestDrawDisplay(void);
void DrawBitmap(const uint8_t *bitmap);

#endif