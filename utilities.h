// Определение пинов для различных версий модема SIM800
#if defined(SIM800L_IP5306_VERSION_20190610)

#define MODEM_RST             5       // Пин для сброса модема
#define MODEM_PWRKEY          4       // Пин для включения/выключения питания
#define MODEM_POWER_ON       23       // Пин для включения питания модема
#define MODEM_TX             27       // Пин для передачи данных (TX)
#define MODEM_RX             26       // Пин для приема данных (RX)

#define I2C_SDA              21      // Пин для SDA шины I2C
#define I2C_SCL              22      // Пин для SCL шины I2C
#define LED_GPIO             13      // Пин для индикатора (светодиода)
#define LED_ON               HIGH    // Состояние включенного светодиода
#define LED_OFF              LOW     // Состояние выключенного светодиода

#define IP5306_ADDR          0x75    // Адрес контроллера питания IP5306
#define IP5306_REG_SYS_CTL0  0x00    // Регистор для управления питанием

// Функция настройки управления питанием (PMU) для IP5306
bool setupPMU()
{
    bool en = true; // Включение boost (увеличение напряжения)
    Wire.begin(I2C_SDA, I2C_SCL);  // Инициализация I2C шины
    Wire.beginTransmission(IP5306_ADDR);  // Начало передачи по I2C
    Wire.write(IP5306_REG_SYS_CTL0); // Выбор регистра управления питанием
    if (en) {
        Wire.write(0x37); // Включение boost keep on
    } else {
        Wire.write(0x35); // Отключение boost keep on
    }
    return Wire.endTransmission() == 0; // Завершаем передачу и проверяем успешность
}

#elif defined(SIM800L_AXP192_VERSION_20200327)

#define MODEM_RST            5       // Пин для сброса модема
#define MODEM_PWRKEY          4      // Пин для включения/выключения питания
#define MODEM_POWER_ON       23      // Пин для включения питания модема
#define MODEM_TX             27      // Пин для передачи данных (TX)
#define MODEM_RX             26      // Пин для приема данных (RX)
#define MODEM_DTR            32      // Пин для Data Terminal Ready (не используется в данном коде)
#define MODEM_RI             33      // Пин для Ring Indicator (не используется в данном коде)

#define I2C_SDA              21      // Пин для SDA шины I2C
#define I2C_SCL              22      // Пин для SCL шины I2C
#define LED_GPIO             13      // Пин для индикатора (светодиода)
#define LED_ON               HIGH    // Состояние включенного светодиода
#define LED_OFF              LOW     // Состояние выключенного светодиода

#elif defined(SIM800C_AXP192_VERSION_20200609)
// Пиновые определения для SIM800C

#define MODEM_PWRKEY          4      // Пин для включения/выключения питания
#define MODEM_POWER_ON       25      // Пин для включения питания модема
#define MODEM_TX             27      // Пин для передачи данных (TX)
#define MODEM_RX             26      // Пин для приема данных (RX)
#define MODEM_DTR            32      // Пин для Data Terminal Ready
#define MODEM_RI             33      // Пин для Ring Indicator

#define I2C_SDA              21      // Пин для SDA шины I2C
#define I2C_SCL              22      // Пин для SCL шины I2C
#define LED_GPIO             12      // Пин для индикатора (светодиода)
#define LED_ON               LOW     // Состояние включенного светодиода
#define LED_OFF              HIGH    // Состояние выключенного светодиода

#elif defined(SIM800L_IP5306_VERSION_20200811)

#define MODEM_RST             5      // Пин для сброса модема
#define MODEM_PWRKEY          4      // Пин для включения/выключения питания
#define MODEM_POWER_ON       23      // Пин для включения питания модема
#define MODEM_TX             27      // Пин для передачи данных (TX)
#define MODEM_RX             26      // Пин для приема данных (RX)

#define MODEM_DTR            32      // Пин для Data Terminal Ready
#define MODEM_RI             33      // Пин для Ring Indicator

#define I2C_SDA              21      // Пин для SDA шины I2C
#define I2C_SCL              22      // Пин для SCL шины I2C
#define LED_GPIO             13      // Пин для индикатора (светодиода)
#define LED_ON               HIGH    // Состояние включенного светодиода
#define LED_OFF              LOW     // Состояние выключенного светодиода

#define IP5306_ADDR          0x75    // Адрес контроллера питания IP5306
#define IP5306_REG_SYS_CTL0  0x00    // Регистор для управления питанием

// Функция настройки управления питанием (PMU) для IP5306
bool setupPMU()
{
    bool en = true; // Включение boost (увеличение напряжения)
    Wire.begin(I2C_SDA, I2C_SCL);  // Инициализация I2C шины
    Wire.beginTransmission(IP5306_ADDR);  // Начало передачи по I2C
    Wire.write(IP5306_REG_SYS_CTL0); // Выбор регистра управления питанием
    if (en) {
        Wire.write(0x37); // Включение boost keep on
    } else {
        Wire.write(0x35); // Отключение boost keep on
    }
    return Wire.endTransmission() == 0; // Завершаем передачу и проверяем успешность
}

#else

#error "Please select the corresponding model" // Ошибка, если не выбрана модель

#endif


// Включение библиотеки для работы с контроллером питания AXP192
#if defined(SIM800L_AXP192_VERSION_20200327) || defined(SIM800C_AXP192_VERSION_20200609)
#include <axp20x.h>  // Библиотека для работы с AXP192

AXP20X_Class axp;  // Создание объекта для управления питанием

// Функция настройки управления питанием для AXP192
bool setupPMU()
{
    Wire.begin(I2C_SDA, I2C_SCL);  // Инициализация I2C шины
    int ret = axp.begin(Wire, AXP192_SLAVE_ADDRESS);  // Инициализация AXP192

    if (ret == AXP_FAIL) {
        Serial.println("AXP Power begin failed");  // Если инициализация не удалась
        return false;
    }

    // Отключаем неиспользуемые выходы питания
    axp.setPowerOutPut(AXP192_DCDC1, AXP202_OFF);
    axp.setPowerOutPut(AXP192_LDO2, AXP202_OFF);
    axp.setPowerOutPut(AXP192_LDO3, AXP202_OFF);
    axp.setPowerOutPut(AXP192_DCDC2, AXP202_OFF);
    axp.setPowerOutPut(AXP192_EXTEN, AXP202_OFF);

    // Получаем информацию о напряжении
    float vbus_v = axp.getVbusVoltage();
    float vbus_c = axp.getVbusCurrent();
    float batt_v = axp.getBattVoltage();  // Напряжение батареи
    // axp.getBattPercentage(); // AXP192 не поддерживает процент заряда
    Serial.printf("VBUS:%.2f mV %.2f mA ,BATTERY: %.2f\n", vbus_v, vbus_c, batt_v);

    return true;  // Возвращаем успех
}

#endif

// Функция для инициализации модема
void setupModem()
{
    // Настройка управления питанием
    if (setupPMU() == false) {
        Serial.println("Setting power error");  // Ошибка настройки питания
    }

#ifdef MODEM_RST
    // Подключаем пин сброса (если есть)
    pinMode(MODEM_RST, OUTPUT);
    digitalWrite(MODEM_RST, HIGH);  // Поднимаем пин сброса
#endif

    // Настройка пинов для включения модема
    pinMode(MODEM_PWRKEY, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);

    // Включаем питание модема
    digitalWrite(MODEM_POWER_ON, HIGH);

    // Понижаем PWRKEY для включения модема
    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(100);  // Задержка
    digitalWrite(MODEM_PWRKEY, LOW);  // Ожидание
    delay(1000);  // Задержка
    digitalWrite(MODEM_PWRKEY, HIGH);  // Повторное включение

    // Инициализация индикатора состояния
    pinMode(LED_GPIO, OUTPUT);
    digitalWrite(LED_GPIO, LED_OFF);  // Изначально индикатор выключен
}
