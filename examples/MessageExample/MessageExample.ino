/*
  MessageExample - пример использования класса message<N>
  для упаковки разных типов данных в один пакет.
  
  Этот скетч показывает, как использовать шаблонный класс message
  для создания и отправки пакетов с разными типами данных.
*/

#include <easyEthernetLib.h>

// MAC-адрес платы
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Порт для обмена данными
unsigned int localPort = 8888;

// Создаём объект передатчика
DataTransmitter dt(mac, localPort, "ARDUINO");

// Структура с данными датчиков
struct SensorData {
  float temperature;
  int humidity;
  unsigned long timestamp;
};

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial.println(F("EasyEthernetLib - Message Example"));
  
  // инициализация через статический IP
  IPAddress ip(192, 168, 1, 178);  // IP отличается от отправителя
  if (dt.init(ip) != 0)
  {
    DEBUG_ERROR("Init failed");
    while(1);
  }
  
  // Настраиваем широковещательную отправку
  dt.setBroadcastTargetIP();
}

void loop() {
  
  // Создаём буфер для сообщения
  message<128> msg;
  
  // 1. Добавляем строку-заголовок (автоматически добавится завершающий 0)
  msg.push("Sensor Data Packet");
  
  // 2. Создаём и заполняем структуру с данными
  SensorData data = {
    .temperature = 23.5,
    .humidity = 45,
    .timestamp = millis()
  };
  
  // 3. Добавляем структуру целиком
  msg.push(data);
  
  // 4. Добавляем отдельное значение
  uint16_t batteryMv = 3300;  // 3.3V
  msg.push(batteryMv);
  
  // Отправляем сформированный пакет
  dt.sendData(msg);
  
  // Теперь попробуем принять данные
  message<128> rxMsg;
  dt.receiveData(&rxMsg);

  
  if (rxMsg.getSize() > 0) {
    Serial.println(F("\nReceived packet:"));
    
    // 1. Читаем заголовок
    char* header = rxMsg.readString();
    if(header)
      Serial.println(header);
    else
      Serial.println(F("Error reading header"));

    // 2. Читаем структуру с данными
    const SensorData* rxData = rxMsg.read<SensorData>();
    Serial.print(F("Temperature: "));
    Serial.println(rxData->temperature);
    Serial.print(F("Humidity: "));
    Serial.println(rxData->humidity);
    Serial.print(F("Timestamp: "));
    Serial.println(rxData->timestamp);

    // 3. Читаем значение батареи
    int charge = rxMsg.read<uint16_t>();
    Serial.print(F("Battery: ")); Serial.println(charge);


  }
  
  delay(1000);
}