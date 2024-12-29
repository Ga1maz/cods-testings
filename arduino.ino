#include "TFLidar.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11);  // Пины TX и RX
#define LIDAR_SERIAL mySerial

TFLidar lidar;
int dist;  // Переменная для хранения текущего расстояния
int lastDist = 0;  // Переменная для хранения последнего значения расстояния
int threshold = 10;  // Порог для изменения расстояния

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial.print("Serial init OK\r\n");
  
  LIDAR_SERIAL.begin(115200);
  lidar.begin(&LIDAR_SERIAL);
  
  pinMode(13, OUTPUT);  // Устанавливаем пин 13 как выход
  digitalWrite(13, LOW);  // Изначально пин 13 выключен
}

void loop() {
  lidar.getData(dist);
  
  // Проверяем, изменилось ли расстояние более чем на 10 см
  if (abs(dist - lastDist) > threshold) {
    // Если изменилось, включаем пин 13
    digitalWrite(13, HIGH);
  } else {
    // Иначе выключаем пин 13
    digitalWrite(13, LOW);
  }

  // Отображаем текущие данные в Serial
  Serial.print("Dist: ");
  Serial.print(dist);
  Serial.print(" cm");
  Serial.println();
  
  // Обновляем последнее значение расстояния
  lastDist = dist;
  
  delay(100);
}
