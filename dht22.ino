#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 15           // Пин для подключения DHT22
#define DHTTYPE DHT22       // Используем датчик DHT22

const char* ssid = "dom100a";
const char* password = "almaz@7777";
const char* mqtt_server = "mqtt.cloa.su";
const char* mqtt_user = "ga1maz";
const char* mqtt_pass = "almazg1234";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

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
  dht.begin();  // Инициализация датчика DHT22

  setupWiFi();
  client.setServer(mqtt_server, mqtt_port);

  Serial.println("Инициализация завершена");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Получаем данные с DHT22
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Ошибка чтения с DHT22");
    return;
  }

  // Отправка данных в MQTT
  char temp_str[8];
  snprintf(temp_str, sizeof(temp_str), "%.2f", temperature);
  client.publish("dht22/temperature", temp_str);

  char hum_str[8];
  snprintf(hum_str, sizeof(hum_str), "%.2f", humidity);
  client.publish("dht22/humidity", hum_str);

  // Печатаем данные в Serial Monitor
  Serial.printf("Температура: %.2f °C, Влажность: %.2f %%\n", temperature, humidity);

  delay(2000);  // Период опроса датчика DHT22
}
