## EasyEthernetLib

Простая библиотека для обмена данными по Ethernet/UDP с Arduino (на базе Ethernet и EthernetUdp). Протестирована на W5500.

Особенности:
- Отправка/приём сырых байтов и C-строк
- Удобный шаблонный буфер `message<N>` для накопления данных
- Поддержка задания целевого IP (включая широковещательный)
- Небольшой API, совместимый с Arduino Ethernet библиотекой

Установка
- Скачать исходный код в виде .zip и в Arduino IDE в разделе "скетч/подключить библиотеку" нажать добавить .ZIP библиотеку.


Подключение в скетче
```cpp
#include <EasyEthernetLib.h>

// пример MAC-адреса платы
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// создаём передатчик: mac, порт, magicString (можно использовать пустую строку)
DataTransmitter dt(mac, 8888, "MAGIC");

void setup() {
  Serial.begin(115200);

  // Инициализация: нужно задать статический IP
   dt.init(IPAddress(192,168,1,50));

  /* 
  По умолчанию второе устройство находится автоматически, если у них совпадают магические строки,
  при получении пакеты, если магическая строка совпадает, а IP адрес не совпадает, то целевой IP аддрес обновляется
  но можно либо всегда использовать brodcast, всегда отправляя пакеты всем устройствам в сети, 
  устройства будут принимать пакеты, только если у них совпадают магические строки 
  */
  //dt.setBroadcastTargetIP(); 
  /*
  Так же можно установить целевое устройво по IP адресу, по умолчанию таким образом устройство фиксируется
  и целевой IP адрес не будет меняться
  */
  //dt.setTargetIP(IPAddress(192,168,1,100));
}

void loop() {
  // отправка C-строки
  dt.sendData("Hello from Arduino");

  // использование message для сборки пакета
  message<128> buf;
  const char* txt = "Payload";
  buf.push(txt); // добавляет строку с нулём
  int left = dt.sendData(buf); // отправляет содержимое буфера

  delay(1000);
}
```

Краткое описание API

- Класс: `DataTransmitter`
  - Конструктор: `DataTransmitter(const byte* mac, unsigned int port, const char* magicString);`
  - Инициализация: `int init(IPAddress ip);` (есть вариант без аргументов, закомментирован в заголовке)
  - setTargetIP / setBroadcastTargetIP / resetTargetIP
  - setLockTargetIP(bool)
  - isValid() — проверка состояния
  - sendData(const byte* data, int dataSize)
  - sendData(const char* data)
  - template<int N> sendData(const message<N>& data)
  - receiveData(byte* buffer, int maxSize) -> size_t
  - template<int N> receiveData(message<N>* buffer)
  - getTargetIP(), getIP()
  - maintain() — обновление состояния (например, DHCP)

- Шаблонный буфер: `message<N>` (см. `src/message.h`)
  - Методы: `getSize()`, `getCapacity()`, `getSpace()`, `getData()`, `getEnd()`
  - `push(const uint8_t*, size_t)`, `push(const char*)`, `template push(T&)`
  - `clear()`

Файлы в репозитории
- `easyEthernetLib.h` — основной заголовок (API класса `DataTransmitter`)
- `easyEthernetLib.cpp` — реализация
- `message.h` — шаблонный буфер `message<N>` для упаковки данных
- `utils.h` — утилиты (макросы и пр.)
# Библиотека для передачи данных между Arduino
