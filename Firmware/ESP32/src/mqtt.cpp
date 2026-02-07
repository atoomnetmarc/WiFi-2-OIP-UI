/*

Copyright 2024-2026 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncMqttClient.h>
#include <Ticker.h>
#include <WiFi.h>

#include "Event.h"
#include "config.h"
#include "display.h"
#include "main.h"
#include "mqtt.h"

// http://marvinroger.viewdocs.io/async-mqtt-client/2.-API-reference/
AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
String baseTopic;
float mqttReconnectTimerTime = 0;

void connectToMqtt() {
    Serial.print(F("Attempting MQTT connection to "));
    Serial.print(mqtt_host);
    Serial.println(F("..."));

    mqttClient.connect();
}

void MqttOnWifiConnect() {
    connectToMqtt();
}

void MqttOnWifiDisconnect() {
    mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
}

void MqttPublishStuff() {
    MqttPublish(F("state/esp/ChipCores"), ESP.getChipCores());
    MqttPublish(F("state/esp/ChipModel"), ESP.getChipModel());
    MqttPublish(F("state/esp/ChipRevision"), ESP.getChipRevision());
    MqttPublish(F("state/esp/CpuFreqMHz"), ESP.getCpuFreqMHz());
    MqttPublish(F("state/esp/FlashChipMode"), ESP.getFlashChipMode());
    MqttPublish(F("state/esp/FlashChipSize"), ESP.getFlashChipSize());
    MqttPublish(F("state/esp/FlashChipSpeed"), ESP.getFlashChipSpeed());
    MqttPublish(F("state/esp/FreeSketchSpace"), ESP.getFreeSketchSpace());
    MqttPublish(F("state/esp/SdkVersion"), ESP.getSdkVersion());
    MqttPublish(F("state/esp/SketchMD5"), ESP.getSketchMD5());
    MqttPublish(F("state/esp/SketchSize"), ESP.getSketchSize());

    MqttPublish(F("state/wifi/macAddress"), WiFi.macAddress());
    MqttPublish(F("state/wifi/BSSID"), WiFi.BSSIDstr());
    MqttPublish(F("state/wifi/SSID"), WiFi.SSID());
    MqttPublish(F("state/wifi/RSSI"), WiFi.RSSI());
    MqttPublish(F("state/wifi/channel"), WiFi.channel());
    MqttPublish(F("state/wifi/localIP"), WiFi.localIP().toString());
}

void onMqttConnect(bool sessionPresent) {
    mqttReconnectTimerTime = 2;
    Serial.println(F("Mqtt connected"));
    // Subscribe
    String commandTopic = String(baseTopic + String(F("command")));
    Serial.print(F("Subscribing to "));
    Serial.println(commandTopic);
    mqttClient.subscribe(commandTopic.c_str(), 0);

    String bitmapTopic = String(baseTopic + String(F("bitmap")));
    Serial.print(F("Subscribing to "));
    Serial.println(bitmapTopic);
    mqttClient.subscribe(bitmapTopic.c_str(), 0);

    MqttPublish(F("state/current"), F("online"));

    MqttPublishStuff();
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    Serial.println(F("Got disconnected from MQTT"));

    if (WiFi.isConnected()) {
        if (mqttReconnectTimerTime < 60) {
            mqttReconnectTimerTime += 1;
        }

        mqttReconnectTimer.once(mqttReconnectTimerTime, connectToMqtt);
    }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
    /*
    Serial.println(F("Subscribe acknowledged."));
    Serial.print(F("  packetId: "));
    Serial.println(packetId);
    Serial.print(F("  qos: "));
    Serial.println(qos);
    */
}

void onMqttUnsubscribe(uint16_t packetId) {
    /*
    Serial.println(F("Unsubscribe acknowledged."));
    Serial.print(F("  packetId: "));
    Serial.println(packetId);
    */
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    Serial.print(F("Message arrived on topic: "));
    Serial.println(topic);

    if (String(topic) == String(baseTopic + F("command"))) {
        Serial.print(F(". Message: "));
        String json;

        for (unsigned int i = 0; i < len; i++) {
            Serial.print((char)payload[i]);
            json += (char)payload[i];
        }
        Serial.println();
        event.ProcessJson(json);
    }

    if (String(topic) == String(baseTopic + F("bitmap"))) {
        Serial.println(F("We got send a bitmap."));
        DrawBitmap((uint8_t *)payload);
    }
}

void onMqttPublish(uint16_t packetId) {
    /*
    Serial.println("Publish acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
    */
}

bool MqttIsConnected() {
    return mqttClient.connected();
}

void MqttLoop() {
    // No loop() needed for AsyncMqttClient :-)
}

String willTopic;
void MqttSetup() {
    String mac = WiFi.macAddress();
    mac.replace(F(":"), F(""));
    mac.toLowerCase();
    baseTopic = "lcd/" + mac + "/";
    Serial.print(F("Setting MQTT server to "));
    Serial.println(mqtt_host);

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);

    mqttClient.setCleanSession(true);

    static String clientid = "lcd-" + mac;
    mqttClient.setClientId(clientid.c_str());
    mqttClient.setCredentials(mqtt_user, mqtt_pass);
    willTopic = baseTopic + String(F("state/current"));
    mqttClient.setWill(willTopic.c_str(), 0, true, "offline");

    mqttClient.setServer(mqtt_host, 1883);
}

void MqttPublish(const char *topic, const char *value) {
    mqttClient.publish(topic, 0, true, value);
}

void MqttPublish(String topic, int32_t value) {
    MqttPublish(topic, String(value));
}

void MqttPublish(String topic, String value) {
    MqttPublish(topic, value.c_str());
}

void MqttPublish(String topic, const char *value) {
    MqttPublish(String(baseTopic + topic).c_str(), value);
}
