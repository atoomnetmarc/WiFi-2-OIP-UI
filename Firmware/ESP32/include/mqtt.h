/*

Copyright 2024-2025 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#ifndef mqtt_h
#define mqtt_h

void MqttLoop(void);
void MqttSetup(void);

void MqttPublish(String topic, int32_t value);
void MqttPublish(String topic, String value);
void MqttPublish(String topic, const char *value);

void MqttOnWifiConnect();
void MqttOnWifiDisconnect();
void MqttPublishStuff();

bool MqttIsConnected();

#endif