/*
  BasicSender - простой пример отправки данных через UDP
  используя библиотеку EasyEthernetLib.
  
  Этот скетч отправляет текстовое сообщение каждую секунду
  на заданный IP-адрес (или широковещательно).
*/

#include <easyEthernetLib.h>

// MAC-адрес вашей платы Arduino Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Порт для отправки данных
unsigned int localPort = 8888;

// Создаём объект передатчика
// Параметры: MAC-адрес, порт, "магическая строка" (для фильтрации пакетов)
DataTransmitter dt(mac, localPort, "ARDUINO");

// Счётчик отправленных пакетов
unsigned long messageCount = 0;

void setup() {
  // Инициализация последовательного порта для отладки
  Serial.begin(9600);
  while (!Serial) {
    ; // Ждём подключения последовательного порта (только для Leonardo)
  }
  
  Serial.println(F("EasyEthernetLib - Basic Sender Example"));
  
  // Инициализация через статический IP
  IPAddress ip(192, 168, 1, 177);
  if (dt.init(ip) != 0)
  {
    DEBUG_ERROR("Init failed");
    while(1);
  }
  
  // Так как другое устройство не отправляет пакеты, это устройство не сможет узнать его IP,
  // поэтому всегда используем широковещательную отправку. 
  // Настраиваем целевой IP для отправки
  // Вариант 1: широковещательная отправка (на все устройства в сети)
  dt.setBroadcastTargetIP(); // На самом деле это просто блокирует обновление целевого IP и ставит его в 255.255.255.255
  
  /* Вариант 2: если известен IP другого устройства, то можно отправлять на конкретный IP
  dt.setTargetIP(IPAddress(192, 168, 1, 100));
  */
}

void loop() {
  
  // Формируем сообщение
  char message[64];
  snprintf(message, sizeof(message), "Hello #%lu from Arduino!", messageCount++);
  
  // Отправляем данные
  int result = dt.sendData(message);
  
  // Выводим информацию об отправке
  Serial.print(F("Sending: "));
  Serial.print(message);
  Serial.print(F(" -> "));
  Serial.print(dt.getTargetIP());
  Serial.print(F(" Result: "));
  Serial.println(result);
  
  // Пауза перед следующей отправкой
  delay(1000);
}