#include <Wire.h>
#include <Adafruit_INA219.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Настройки Wi-Fi
const char* ssid = "dom100a";
const char* password = "almaz@7777";

// Настройки MQTT
const char* mqtt_server = "mqtt.cloa.su";
const char* mqtt_user = "ga1maz";
const char* mqtt_password = "almazg1234";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_INA219 ina219;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Соединение с Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi подключено");
  Serial.print("IP адрес: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Подключение к MQTT серверу...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Подключено!");
    } else {
      Serial.print("Ошибка, rc=");
      Serial.print(client.state());
      Serial.println(" Повтор через 5 секунд");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!ina219.begin()) {
    Serial.println("INA219 не найден. Проверьте соединение.");
    while (1);
  }
  Serial.println("INA219 инициализирован.");

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float busvoltage = ina219.getBusVoltage_V(); // Напряжение шины
  float current_mA = ina219.getCurrent_mA();   // Ток в миллиамперах
  float power_mW = ina219.getPower_mW();       // Мощность в милливаттах
  float loadvoltage = busvoltage + (current_mA / 1000 * ina219.getShuntVoltage_mV() / 1000);

  // Отправка данных в MQTT
  char msg[50];

  snprintf(msg, sizeof(msg), "%.2f", busvoltage);
  client.publish("ina219/volt", msg);

  snprintf(msg, sizeof(msg), "%.2f", current_mA);
  client.publish("ina219/current", msg);

  snprintf(msg, sizeof(msg), "%.2f", loadvoltage);
  client.publish("ina219/loadvoltage", msg);

  snprintf(msg, sizeof(msg), "%.2f", power_mW);
  client.publish("ina219/power", msg);

  Serial.println("Данные отправлены на MQTT");
  delay(2000); // Задержка перед следующим измерением
}
