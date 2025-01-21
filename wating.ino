#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define TEMP_THRESHOLD 15

#define DHTPIN 4           // Пин для DHT22
#define FAN_PIN 5          // Пин для вентилятора
#define HEATER_PIN 15      // Пин для нагревательного элемента

#define OLED_ADDR 0x3C
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(128, 64, &Wire);

void setup() {
  pinMode(FAN_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(HEATER_PIN, LOW);

  dht.begin();

  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 not found"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  float temp = dht.readTemperature();

  if (isnan(temp)) {
    displayError("DHT Error");
    delay(2000);
    return;
  }

  String fanState = "OFF";
  String heaterState = "OFF";

  if (temp > TEMP_THRESHOLD) {
    digitalWrite(FAN_PIN, HIGH);
    digitalWrite(HEATER_PIN, LOW);
    fanState = "ON";
  } else {
    digitalWrite(FAN_PIN, LOW);
    digitalWrite(HEATER_PIN, HIGH);
    heaterState = "ON";
  }
  displayData(temp, fanState, heaterState);

  delay(2000);
}

void displayData(float temp, String fan, String heater) {
  display.clearDisplay();
  display.setCursor(0, 0);

  display.print("Temperature: ");
  display.println(temp);
  display.print("Fan: ");
  display.println(fan);
  display.print("Heat: ");
  display.println(heater);

  display.display();
}

void displayError(String errorMsg) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Error: ");
  display.println(errorMsg);
  display.display();
}
