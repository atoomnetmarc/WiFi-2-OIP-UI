/*

Copyright 2024-2026 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#ifndef Event_h
#define Event_h

#include <Arduino.h>
#include <ArduinoJson.h>

class Event {
public:
    Event(void);
    void ProcessJson(String json);

private:
    JsonDocument _doc;
};

extern Event event;

#endif