#include <SPI.h>
#include <Ethernet.h>
#include <WebSocketClient.h>
#include <ArduinoJson.h>

byte mac[]    = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "just-server-production.herokuapp.com";

WebSocketClient client;

void setup() {
  Serial.begin(9600); while (!Serial) { }

  establishInternetConnection();
  establishWebSocketConnection();

  client.send("{\"d\":\"hello\"}");
}

const unsigned long reconnectInterval = 5L * 1000L;
unsigned long lastConnectionTime      = 0;

void loop() {
  if (client.connected()) {
    client.monitor();
  } else if (millis() - lastConnectionTime > reconnectInterval) {
    establishInternetConnection();
    establishWebSocketConnection();

    // remember connection time...

    lastConnectionTime = millis();
  }
}

void establishInternetConnection() {
  Serial.println("Establishing Internet connection...");

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }

  delay(1000);
}

void establishWebSocketConnection() {
  Serial.println("Establishing WebSocket connection...");

  client.disconnect();

  if (!client.connect(server)) {
    Serial.println("Failed to initialize WebSocket");
  }

  client.setDataArrivedDelegate(onMessage);
}

StaticJsonBuffer<200> jsonBuffer;

void onMessage(WebSocketClient client, String message) {
  Serial.println("Message received: " + message);

  char json[message.length() + 1]; message.toCharArray(json, message.length() + 1);

  JsonObject &parsed = jsonBuffer.parseObject(json);

  if (parsed.success()) {
    Serial.println("Data is valid JSON");
  }
}

