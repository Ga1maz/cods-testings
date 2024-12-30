#include <ESP8266WiFi.h>         // Подключение библиотеки для работы с Wi-Fi
#include <PubSubClient.h>         // Подключение библиотеки для работы с MQTT
#include <SoftwareSerial.h>       // Подключение библиотеки для программного последовательного порта
#include <TinyGPS++.h>            // Подключение библиотеки для работы с GPS

// Указать данные для подключения к Wi-Fi и MQTT
const char* ssid = "***********";               // SSID Wi-Fi сети
const char* password = "*****";                  // Пароль Wi-Fi сети
const char* mqtt_server = "**********";          // Адрес MQTT сервера
const char* mqtt_user = "*****";                 // Имя пользователя для MQTT
const char* mqtt_password = "**********";        // Пароль для MQTT

WiFiClient espClient;                            // Объект для подключения по Wi-Fi
PubSubClient client(espClient);                  // Объект для работы с MQTT
TinyGPSPlus gps;                                // Объект для работы с GPS
SoftwareSerial GPSSerial(4, 5);                  // Настройка программного последовательного порта (RX: D2, TX: D1)

unsigned long lastSendTime = 0;                  // Время последней отправки координат
const unsigned long sendInterval = 500;          // Интервал между отправками координат (в миллисекундах)

void setup() {
    Serial.begin(115200);                        // Инициализация серийного порта для отладки
    GPSSerial.begin(9600);                       // Инициализация GPS-модуля (9600 бод)
    WiFi.begin(ssid, password);                  // Подключение к Wi-Fi сети
    while (WiFi.status() != WL_CONNECTED) {      // Ожидание подключения к Wi-Fi
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");         // Выводим сообщение о подключении к Wi-Fi
    client.setServer(mqtt_server, 1883);         // Настройка MQTT сервера (порт 1883)
}

void reconnect() {
    while (!client.connected()) {                // Пока не подключены к MQTT серверу
        if (client.connect("ESP8266GPS", mqtt_user, mqtt_password)) {   // Подключаемся с заданными учетными данными
            Serial.println("Connected to MQTT");  // Сообщение о подключении к MQTT
        } else {
            delay(5000);                         // Ожидание 5 секунд перед повторной попыткой
        }
    }
}

void loop() {
    if (!client.connected()) {                   // Если MQTT-соединение не установлено
        reconnect();                              // Переподключаемся
    }

    client.loop();                               // Обработка входящих сообщений MQTT
    while (GPSSerial.available() > 0) {           // Пока есть данные от GPS
        gps.encode(GPSSerial.read());             // Обработка данных GPS
    }

    // Проверка на обновление координат GPS
    if (gps.location.isUpdated() && gps.location.isValid()) {
        unsigned long currentMillis = millis();  // Получаем текущее время в миллисекундах
        if (currentMillis - lastSendTime >= sendInterval) {   // Если прошло достаточно времени для отправки
            lastSendTime = currentMillis;         // Обновляем время последней отправки
            float latitude = gps.location.lat();  // Получаем широту
            float longitude = gps.location.lng(); // Получаем долготу
            String coordinates = String(latitude, 6) + "," + String(longitude, 6); // Формируем строку с координатами
            client.publish("gps/coordinates", coordinates.c_str()); // Отправка координат в топик MQTT
            Serial.println("Sent: " + coordinates);  // Выводим координаты в консоль для отладки
        }
    }
    delay(10);  // Небольшая задержка для предотвращения излишней загрузки процессора
}
