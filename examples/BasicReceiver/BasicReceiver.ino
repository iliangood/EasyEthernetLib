/*
  BasicReceiver - простой пример приёма данных через UDP
  используя библиотеку EasyEthernetLib.
  
  Этот скетч принимает UDP-пакеты и выводит их содержимое
  в последовательный порт. Работает в паре с BasicSender.
*/

#include <easyEthernetLib.h>

// MAC-адрес вашей платы Arduino Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };  // Немного отличается от отправителя

// Порт для приёма данных (такой же, как у отправителя)
unsigned int localPort = 8888;

// Создаём объект передатчика с тем же магическим словом
DataTransmitter dt(mac, localPort, "ARDUINO");

// Буфер для приёма данных
char packetBuffer[128];

void setup() {
  // Инициализация последовательного порта
  Serial.begin(9600);
  while (!Serial) {
    ; // Ждём подключения последовательного порта (только для Leonardo)
  }
  
  Serial.println(F("EasyEthernetLib - Basic Receiver Example"));
  
  
  // инициализация через статический IP
  IPAddress ip(192, 168, 1, 178);  // IP отличается от отправителя
  if (dt.init(ip) != 0)
  {
    DEBUG_ERROR("Init failed");
    while(1);
  }
  
  Serial.println(F("Waiting for UDP packets..."));
}

void loop() {
  
  // Пробуем принять данные
  size_t packetSize = dt.receiveData((byte*)packetBuffer, sizeof(packetBuffer)).dataSize;
  
  if (packetSize > 0) {
    // Получили что-то - выводим информацию
    IPAddress remoteIP = dt.getTargetIP();
    
    Serial.print(F("Received packet from "));
    Serial.print(remoteIP);
    Serial.print(F(", size: "));
    Serial.print(packetSize);
    Serial.print(F(" bytes: "));
    
    // Добавляем нулевой символ в конец для корректного вывода строки
    packetBuffer[packetSize] = 0;
    Serial.println(packetBuffer);
  }
  
  // Небольшая задержка между проверками
  delay(10);
}