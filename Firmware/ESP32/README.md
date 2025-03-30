# WiFi-2-OIP-UI ESP32

This is a proof of concept project that connects to WiFi, then connects to an MQTT broker via WebSockets. It publishes some device information to the MQTT broker, and subscribes to the `clockall/solartime` topic, displaying the payload on a small display.

## Configuration

1.  Copy `include/config-example.h` to `include/config.h`.
2.  Edit `include/config.h` and fill in your WiFi credentials and MQTT over websocket configuration.