/*

Copyright 2024-2026 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include <Arduino.h>

#ifndef config_h
#define config_h

// WiFi configuration.
extern const char *ssid1;
extern const char *password1;

// Another WiFi configuration.
extern const char *ssid2;
extern const char *password2;

// OTA configuration.
extern const char *otapassword;

// MQTT configuration.
extern const char *mqtt_host;
extern const char *mqtt_user;
extern const char *mqtt_pass;
extern const int mqtt_port;

#define M_INT   0
#define M_RESET 5
#define M_SCK   7
#define M_MOSI  11
#define M_MISO  9
#define M_CS    12
#define LCD_CS  14
#define LCD_DC  4

#endif