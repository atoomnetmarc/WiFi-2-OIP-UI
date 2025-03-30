/*

Copyright 2024-2025 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#ifndef CONFIG_EXAMPLE_H
#define CONFIG_EXAMPLE_H

// WiFi configuration.
const char *ssid = "YOUR_WIFI_SSID";
const char *pass = "YOUR_WIFI_PASSWORD";

// MQTT over websocket configuration.
const char *mqtt_host = "YOUR_MQTT_HOST";
const char *mqtt_user = "YOUR_MQTT_USER";
const char *mqtt_pass = "YOUR_MQTT_PASSWORD";
const int mqtt_port = 1883;
const char *mqtt_path = "YOUR_MQTT_PATH";

#endif