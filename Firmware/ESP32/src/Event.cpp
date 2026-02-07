
/*

Copyright 2024-2026 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include "Event.h"
#include "display.h"
#include "mqtt.h"

Event event;

Event::Event() {
}

void Event::ProcessJson(String json) {
    DeserializationError error = deserializeJson(_doc, json);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    // TODO: Do something with the json.
}
