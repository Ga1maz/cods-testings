#define PIN_TRIGGER 5  // Пин для проверки 5V

// Определение версии SIM800L модема
#define SIM800L_IP5306_VERSION_20190610
// #define SIM800L_AXP192_VERSION_20200327
// #define SIM800C_AXP192_VERSION_20200609
// #define SIM800L_IP5306_VERSION_20200811

// Определение серийного порта для отладки
#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG          SerialMon

#include "utilities.h"  // Подключение библиотеки для утилит (если есть)

// Настройка серийного порта для вывода отладочных данных (по умолчанию 115200)
#define SerialMon Serial
// Настройка серийного порта для отправки AT-команд модему
#define SerialAT  Serial1

// Настройка библиотеки TinyGSM
#define TINY_GSM_MODEM_SIM800          // Используем модем SIM800
#define TINY_GSM_RX_BUFFER      1024   // Размер буфера RX — 1Кб

#include <TinyGsmClient.h>  // Подключение библиотеки для работы с модемом SIM800

// Определяем использование StreamDebugger для отладки AT-команд
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);  // Создание объекта StreamDebugger
TinyGsm modem(debugger);  // Создание объекта модема с отладчиком
#else
TinyGsm modem(SerialAT);  // Создание объекта модема без отладчика
#endif

// Определение номеров телефонов для тестирования SMS и вызовов
#define SMS_TARGET  "+79631413596"   // Номер для отправки SMS
#define CALL_TARGET "+79631413596"   // Номер для вызова

void setup()
{
    // Устанавливаем скорость обмена для консольного порта
    SerialMon.begin(115200);

    delay(10);  // Короткая задержка для стабилизации

    // Запуск управления питанием
    if (setupPMU() == false) {
        Serial.println("Setting power error");  // Сообщение об ошибке при настройке питания
    }

    // Выполнение начальной настройки модема
    setupModem();

    // Настройка порта для AT-команд модема
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

    // Инициализация пина 5 как входа
    pinMode(PIN_TRIGGER, INPUT);

    delay(6000);  // Задержка перед началом работы
}

void loop()
{
    // Проверка, есть ли на пине 5 напряжение 5V (логическая "высокая" величина)
    if (digitalRead(PIN_TRIGGER) == HIGH) {
        SerialMon.println("Pin 5 is HIGH, proceeding with modem operations...");  // Выводим сообщение, если пин 5 высокий

        // Модем может перезапускаться долго, для пропуска перезапуска можно вызвать init() вместо restart()
        SerialMon.println("Initializing modem...");
        modem.restart();  // Перезапуск модема

        // Переключение аудио каналов
        SerialAT.print("AT+CHFA=1\r\n");
        delay(2);

        // Установка уровня звука звонка
        SerialAT.print("AT+CRSL=100\r\n");
        delay(2);

        // Установка громкости динамика
        SerialAT.print("AT+CLVL=100\r\n");
        delay(2);

        // Включение представления номера вызывающего абонента
        SerialAT.print("AT+CLIP=1\r\n");
        delay(10000);  // Задержка для выполнения команды

        // Выводим номер, на который будем звонить
        Serial.print("Calling:");
        Serial.println(CALL_TARGET);

        // Выполняем звонок
        bool res = modem.callNumber(CALL_TARGET);

        // Выводим результат звонка
        Serial.print("Call:");
        Serial.println(res ? "OK" : "fail");

        // Инициализация модема после звонка
        SerialMon.println("Initializing modem...");
        modem.restart();

        delay(10000);  // Задержка для стабилизации

        // Получаем IMEI модема
        String imei = modem.getIMEI();
        DBG("IMEI:", imei);

        // Отправляем SMS с IMEI
        bool smsRes = modem.sendSMS(SMS_TARGET, String("Hello from ") + imei);
        DBG("SMS:", smsRes ? "OK" : "fail");

        // Задержка перед следующей проверкой пина
        delay(10000);
    } else {
        // Если пин 5 низкий, пропускаем операции с модемом
        SerialMon.println("Pin 5 is LOW, skipping modem operations...");
    }

    // Модем поддерживает работу в фоновом режиме
    modem.maintain();  // Поддержание соединения с модемом
}
