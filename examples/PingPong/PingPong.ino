/*
  PingPong - пример двустороннего обмена данными
  используя библиотеку EasyEthernetLib.
  
  Этот скетч демонстрирует обмен данными между двумя устройствами:
  - Первое устройство отправляет "PING"
  - Второе отвечает "PONG"
  - Измеряется время round-trip (туда и обратно)
  
  Загрузите этот скетч на два устройства, выбрав разные режимы:
  - На первом установите IS_MASTER = true
  - На втором установите IS_MASTER = false
*/

#include <easyEthernetLib.h>

// Выберите режим работы:
const bool IS_MASTER = true;  // true для первого устройства, false для второго

// MAC-адреса устройств (должны быть разными)
byte macMaster[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE1 };
byte macSlave[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE2 };

// IP-адреса (если используется статическая настройка)
IPAddress ipMaster(192, 168, 1, 177);
IPAddress ipSlave(192, 168, 1, 178);

// Порт для обмена данными
unsigned int localPort = 8888;

// Создаём объект передатчика с выбранным MAC
DataTransmitter dt(IS_MASTER ? macMaster : macSlave, localPort, "ARDUINO");

// Буфер для сообщений
message<64> msg;

// Статистика обмена
unsigned long sentCount = 0;
unsigned long receivedCount = 0;
unsigned long totalTime = 0;
unsigned long lastPingTime = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial.print(F("EasyEthernetLib - PingPong Example ("));
  Serial.print(IS_MASTER ? F("MASTER") : F("SLAVE"));
  Serial.println(F(")"));
  
  // Инициализация со статическим IP
  IPAddress ip = IS_MASTER ? ipMaster : ipSlave;
  if (dt.init(ip) != 0)
  {
    DEBUG_ERROR("Init failed");
    while(1);
  }
  
  // Настройка целевого IP не требуется, так как устройства будут узнавать IP друг друга автоматически
  
  Serial.println(F("Ready to start ping-pong!"));
}

void loop() {
  if (IS_MASTER) 
  {
    // MASTER: отправляет PING и ждёт PONG
    if (millis() - lastPingTime >= 1000)  // Пингуем каждую секунду
	{  
      // Очищаем буфер и добавляем PING
      msg.clear();
      msg.push("PING");
      
      // Засекаем время отправки
      lastPingTime = millis();
      
      // Отправляем
      dt.sendData(msg);
      Serial.print(F("Sent PING #"));
      Serial.println(++sentCount);
    }
    
    // Проверяем ответ
    msg.clear();
    dt.receiveData(&msg);
    
    if (msg.getSize() > 0) {
      // Получили ответ - выводим время
      unsigned long rtt = millis() - lastPingTime;
      totalTime += rtt;
      receivedCount++;
      
      Serial.print(F("Got PONG #"));
      Serial.print(receivedCount);
      Serial.print(F(" RTT: "));
      Serial.print(rtt);
      Serial.print(F("ms, Avg: "));
      Serial.print(totalTime / receivedCount);
      Serial.println(F("ms"));
    }
  }
  else {
    // SLAVE: ждёт PING и отвечает PONG
    msg.clear();
    dt.receiveData(&msg);
    
    if (msg.getSize() > 0) {
      // Получили PING - отвечаем PONG
      receivedCount++;
      
      Serial.print(F("Got PING #"));
      Serial.println(receivedCount);
      
      // Отправляем ответ
      msg.clear();
      msg.push("PONG");
      dt.sendData(msg);
      
      Serial.print(F("Sent PONG #"));
      Serial.println(++sentCount);
    }
  }
  
  // Небольшая задержка
  delay(10);
}