#include <easyEthernetLib.h>

// Определите уникальный MAC-адрес для каждого устройства
// Для первого устройства:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// Для второго устройства замените на другой, например: { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };

// Порт и магическая строка (должны быть одинаковыми на обоих устройствах)
const unsigned int PORT = 8888;
const char* MAGIC_STRING = "ARDUINO";

// Создаем объект DataTransmitter
DataTransmitter dt(mac, PORT, MAGIC_STRING);

// Буфер для данных (размер можно увеличить по необходимости)
message<128> buffer;

void setup() {
  // Инициализация Serial для общения с ПК или другим устройством
  Serial.begin(115200);
  while (!Serial); // Ждем подключения Serial
  

  // Инициализация Ethernet с статическим IP
  // Для второго устройства: замените на другой IP, например: IPAddress(192, 168, 1, 51)
  if (dt.init(IPAddress(192, 168, 1, 50)) != 0) {
    Serial.println("Ошибка инициализации Ethernet!");
    while (true); // Остановка, если ошибка
  }

  char ipStr[32];
  snprintf(ipStr, sizeof(ipStr), "%d.%d.%d.%d", dt.getIP()[0], dt.getIP()[1], dt.getIP()[2], dt.getIP()[3]);
  Serial.print("Ethernet инициализирован. IP: ");
  Serial.println(ipStr);
  Serial.println("Ожидание обмена данными...");
}

void loop() {
  // Чтение данных из Serial и отправка в сеть
  if (Serial.available() > 0) {
    buffer.clear(); // Очищаем буфер перед новым чтением
    // Читаем все доступные байты из Serial (до 512 байт)
    size_t bytesRead = Serial.readBytes(buffer.getEnd(), buffer.getSpace());
    if (bytesRead > 0) {
      buffer.addSize(bytesRead); // Обновляем размер буфера
      // Отправляем данные в сеть
      if (dt.sendData(buffer) == 0) {
        Serial.println("Данные отправлены в сеть (" + String(bytesRead) + " байт)");
      } else {
        Serial.println("Ошибка отправки данных!");
      }
    }
  }

  // Получение данных из сети и отправка в Serial
  buffer.clear(); // Очищаем буфер для приема
  dt.receiveData(&buffer); // Получаем данные в буфер (если есть)
  if (buffer.getSize() > 0) {
    // Отправляем полученные данные в Serial
    Serial.write(buffer.getData(), buffer.getSize());
    Serial.println(); // Добавляем новую строку для разделения (опционально)
    Serial.println("Данные получены из сети (" + String(buffer.getSize()) + " байт) и отправлены в Serial");
  }

  // Небольшая задержка для стабильности (можно убрать или изменить)
  delay(10);
}