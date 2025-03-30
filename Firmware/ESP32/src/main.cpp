/*

Copyright 2024-2025 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include <WebSocketsClient.h> // Include before MQTTPubSubClient.h

#include <MQTTPubSubClient.h>
#include <WiFi.h>

#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include "config.h"
U8G2_ST7565_ERC12864_ALT_F_4W_HW_SPI u8g2(U8G2_R0, 14, 4);

WebSocketsClient webSocketsClient;
MQTTPubSubClient mqtt;

/**
 * @brief Connects to WiFi, WebSocket server, and MQTT broker.
 *
 * This function establishes connections to the WiFi network, the WebSocket server,
 * and the MQTT broker. It handles reconnection attempts in case of disconnections.
 */
void connect() {
connect_to_wifi:
    Serial.print("connecting to wifi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" connected!");

connect_to_host:
    Serial.println("connecting to host...");
    webSocketsClient.disconnect();
    // Connect to the WebSocket server using SSL
    webSocketsClient.beginSSL(mqtt_host, mqtt_port, mqtt_path, "", "mqtt");
    webSocketsClient.setReconnectInterval(2000);

    Serial.print("connecting to mqtt broker...");
    String clientid = "wifi2oipui-";
    clientid += WiFi.macAddress();
    // Attempt to connect to the MQTT broker
    while (!mqtt.connect(clientid, mqtt_user, mqtt_pass)) {
        Serial.print(".");
        delay(1000);
        // If WiFi is disconnected, reconnect to WiFi
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi disconnected");
            goto connect_to_wifi;
        }
        // If WebSocket client is disconnected, reconnect to WebSocket server
        if (!webSocketsClient.isConnected()) {
            Serial.println("WebSocketsClient disconnected");
            goto connect_to_host;
        }
    }

    Serial.println(" connected!");

    String baseTopic = "WiFi-2-OIP-UI/" + WiFi.macAddress() + "/";
    // Publish device information to MQTT broker
    mqtt.publish(baseTopic + "BSSID", WiFi.BSSIDstr());
    mqtt.publish(baseTopic + "SSID", WiFi.SSID());
    mqtt.publish(baseTopic + "ChipModel", ESP.getChipModel());
    mqtt.publish(baseTopic + "FlashChipSize", String(ESP.getFlashChipSize()));
    mqtt.publish(baseTopic + "PsramSize", String(ESP.getPsramSize()));
    mqtt.publish(baseTopic + "SdkVersion", ESP.getSdkVersion());
}

/**
 * @brief Arduino setup function.
 *
 * This function is called once at the beginning of the program. It initializes the display,
 * connects to WiFi, initializes the MQTT client, and subscribes to relevant topics.
 */
void setup() {
    setCpuFrequencyMhz(80);
    u8g2.begin();
    u8g2.enableUTF8Print();

    u8g2.clearBuffer();
    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_mystery_quest_36_tn);
    u8g2.setCursor(55, 28);
    u8g2.print("42");
    u8g2.setFont(u8g2_font_busdisplay11x5_tr);
    u8g2.setCursor(30, 45);
    u8g2.print("WiFi-2-OIP-UI");
    u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.setCursor(0, 57);
    u8g2.print("https://github.com/atoomnetmarc/");
    u8g2.sendBuffer();

    Serial.begin(115200);
    WiFi.begin(ssid, pass);

    // Initialize MQTT client
    mqtt.begin(webSocketsClient);

    // Connect to WiFi, host and MQTT broker
    connect();

    // Subscribe to a callback which is called when any packet is received
    mqtt.subscribe([](const String &topic, const String &payload, const size_t size) {
        // Serial.println("mqtt received: " + topic + " - " + payload);
    });

    // Subscribe to the "clockall/solartime" topic and define a callback
    mqtt.subscribe("clockall/solartime", [](const String &payload, const size_t size) {
        // Serial.println(payload);

        u8g2.setFont(u8g2_font_mystery_quest_36_tn);
        u8g2.setFontDirection(0);
        u8g2.clearBuffer();
        u8g2.setCursor(5, 45);
        u8g2.print(payload);
        u8g2.sendBuffer();
    });
}

/**
 * @brief Arduino loop function.
 *
 * This function is called repeatedly in a loop. It updates the MQTT client and reconnects
 * if the connection is lost.
 */
void loop() {
    mqtt.update(); // This function must be called frequently to process incoming messages and maintain the connection.

    // If MQTT client is not connected, reconnect
    if (!mqtt.isConnected()) {
        connect();
    }
}