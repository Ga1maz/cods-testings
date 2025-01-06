#include <Arduino.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <WiFi.h>

#define RXD2 22  // RX пин для TFmini-S
#define TXD2 21  // TX пин для TFmini-S

const char* ssid = "dom100a";
const char* password = "almaz@7777";
const char* mqtt_server = "mqtt.cloa.su";
const char* mqtt_user = "ga1maz";
const char* mqtt_pass = "almazg1234";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
HardwareSerial SerialTFmini(2);

void setupWiFi() {
  delay(10);
  Serial.println("Подключение к WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi подключен");
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Подключение к MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println("MQTT подключен");
    } else {
      Serial.print("Ошибка подключения, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  SerialTFmini.begin(115200, SERIAL_8N1, RXD2, TXD2);

  setupWiFi();
  client.setServer(mqtt_server, mqtt_port);

  Serial.println("Инициализация завершена");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Чтение данных с TFmini-S
  if (SerialTFmini.available()) {
    uint8_t data[9];
    if (SerialTFmini.read() == 0x59 && SerialTFmini.read() == 0x59) {
      for (int i = 0; i < 7; i++) {
        data[i] = SerialTFmini.read();
      }
      uint16_t distance = (data[1] << 8) + data[0];  // Дистанция в мм
      uint16_t strength = (data[3] << 8) + data[2];  // Интенсивность сигнала

      // Отправка данных в MQTT
      char distance_str[8];
      snprintf(distance_str, sizeof(distance_str), "%d", distance);
      client.publish("tfmini/distance", distance_str);

      char strength_str[8];
      snprintf(strength_str, sizeof(strength_str), "%d", strength);
      client.publish("tfmini/strength", strength_str);

      Serial.printf("Дистанция: %d мм, Сила сигнала: %d\n", distance, strength);
    }
  }
  delay(100);  // Период опроса LiDAR
}
