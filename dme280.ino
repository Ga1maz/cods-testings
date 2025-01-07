#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Настройки Wi-Fi
const char* ssid = "Keenetic-4282";         // Укажите имя вашей Wi-Fi сети
const char* password = "ZjrVx8wH"; // Укажите пароль от Wi-Fi

// Настройки MQTT
const char* mqttServer = "192.168.2.53";
const int mqttPort = 1883;
const char* mqttUser = "kirova";             // Логин
const char* mqttPassword = "technopark";     // Пароль
const char* topic = "bme280/data";           // Тема для публикации данных

// Объекты
WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_BME280 bme; // Подключение через I2C (SDA, SCL)

// Таймер для отправки данных
unsigned long lastMsg = 0;
const long interval = 10000; // Интервал в миллисекундах (10 секунд)

void setup() {
  Serial.begin(115200);

  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Подключение к Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi подключен.");
  Serial.println("IP-адрес: " + WiFi.localIP().toString());

  // Подключение к MQTT
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    Serial.println("Подключение к MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Подключено к MQTT.");
    } else {
      Serial.print("Ошибка подключения: ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  // Инициализация BME280
  if (!bme.begin(0x76)) { // Проверьте адрес датчика (0x76 или 0x77)
    Serial.println("Не удалось найти датчик BME280!");
    while (1);
  }
  Serial.println("BME280 успешно инициализирован.");
}

void loop() {
  // Проверка подключения к MQTT
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Отправка данных через заданный интервал
  unsigned long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;
    sendSensorData();
  }
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println("Повторное подключение к MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Подключено к MQTT.");
    } else {
      Serial.print("Ошибка подключения: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void sendSensorData() {
  // Чтение данных с BME280
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F; // Гектопаскали

  // Формирование JSON строки
  String payload = "{";
  payload += "\"temperature\":" + String(temperature, 2) + ",";
  payload += "\"humidity\":" + String(humidity, 2) + ",";
  payload += "\"pressure\":" + String(pressure, 2);
  payload += "}";

  // Публикация данных
  if (client.publish(topic, payload.c_str())) {
    Serial.println("Данные отправлены: " + payload);
  } else {
    Serial.println("Ошибка отправки данных.");
  }
}
