#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

//Указать данные mqtt, и wifi сети
const char* ssid = "***********";
const char* password = "*****";
const char* mqtt_server = "**********";
const char* mqtt_user = "*****";
const char* mqtt_password = "**********";

WiFiClient espClient;
PubSubClient client(espClient);
TinyGPSPlus gps;
SoftwareSerial GPSSerial(4, 5); // RX (D2), TX (D1)

unsigned long lastSendTime = 0; 
const unsigned long sendInterval = 500; 

void setup() {
    Serial.begin(115200);
    GPSSerial.begin(9600);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
    client.setServer(mqtt_server, 1883);
}

void reconnect() {
    while (!client.connected()) {
        if (client.connect("ESP8266GPS", mqtt_user, mqtt_password)) {
            Serial.println("Connected to MQTT");
        } else {
            delay(5000);
        }
    }
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }

    client.loop();
    while (GPSSerial.available() > 0) {
        gps.encode(GPSSerial.read());
    }
    //получение и отправка кординат
    if (gps.location.isUpdated() && gps.location.isValid()) {
        unsigned long currentMillis = millis();
        if (currentMillis - lastSendTime >= sendInterval) {
            lastSendTime = currentMillis; 
            float latitude = gps.location.lat();
            float longitude = gps.location.lng();
            String coordinates = String(latitude, 6) + "," + String(longitude, 6);
            client.publish("gps/coordinates", coordinates.c_str());
            Serial.println("Sent: " + coordinates);
        }
    }
    delay(10); 
}
