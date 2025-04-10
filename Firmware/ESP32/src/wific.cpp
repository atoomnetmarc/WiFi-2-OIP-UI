/*

Copyright 2024-2025 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include <WiFi.h>
#include <WiFiGeneric.h>
#include <Ticker.h>

#include "config.h"
#include "mqtt.h"
#include "ota.h"

Ticker tickerWifiCheckConnection;

String hostname;

esp_netif_t *get_esp_interface_netif(esp_interface_t interface);

void WiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_AP_START:
            // can set ap hostname here
            WiFi.softAPsetHostname(hostname.c_str());
            // enable ap ipv6 here
            WiFi.softAPenableIpV6();
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            // set sta hostname here
            WiFi.setHostname(hostname.c_str());
            Serial.println(F("STA Started"));
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            // enable sta ipv6 here
            Serial.println(F("WiFi connected to: "));
            Serial.println(WiFi.SSID());
            WiFi.setAutoReconnect(true);
            WiFi.enableIpV6();
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            // both interfaces get the same event
            Serial.print("STA Local IPv6: ");
            Serial.println(WiFi.localIPv6().toString());

            Serial.print("STA Global IPv6: ");
            esp_ip6_addr_t addr;
            esp_netif_get_ip6_global(get_esp_interface_netif(ESP_IF_WIFI_STA), &addr);
            Serial.println(IPv6Address(addr.addr));

            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.println("STA IPv4: ");
            Serial.println(WiFi.localIP().toString());
            OTASetup();
            MqttOnWifiConnect();
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println(F("WiFi got disconnected."));
            MqttOnWifiDisconnect();
            WiFi.reconnect();
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            Serial.println(F("STA Stopped"));
            break;
        default:
            break;
    }
}

void WifiBeginNext() {
    static uint8_t ssidCounter = 0;

    if (ssidCounter == 0) {
        WiFi.begin(ssid1, password1);
    } else {
        WiFi.begin(ssid2, password2);
    }

    ssidCounter++;
    if (ssidCounter > 1) {
        ssidCounter = 0;
    }

    WiFi.setAutoReconnect(true);
}

void WifiCheckConnection() {
    if (!WiFi.isConnected()) {
        WifiBeginNext();
    }
}

void WificSetup() {
    String mac = WiFi.macAddress();
    mac.replace(F(":"), F(""));
    mac.toLowerCase();
    hostname = "lcd-" + mac;
    WiFi.setHostname(hostname.c_str());
    WiFi.useStaticBuffers(true);
    WiFi.setSleep(wifi_ps_type_t::WIFI_PS_NONE);
    WiFi.mode(wifi_mode_t::WIFI_STA);

    Serial.println("WiFi settings 1:");
    Serial.println(ssid1);
    Serial.println(password1);

    Serial.println("WiFi settings 2:");
    Serial.println(ssid2);
    Serial.println(password2);

    WifiBeginNext();

    WiFi.onEvent(WiFiEvent);

    tickerWifiCheckConnection.attach_ms(10000, WifiCheckConnection);
}

void WiFicLoop() {
    OTALoop();
}