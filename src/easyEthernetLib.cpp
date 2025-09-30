#include "easyEthernetLib.h"

#include "utils.h"

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#if USE_DHCP
  DataTransmitter::DataTransmitter(const byte* mac, unsigned int port, const char* magicString) 
    : targetIP(255, 255, 255, 255), lockTargetIP(false)
    {
    this->port = port;
    this->magicString = magicString;
    if(magicString != nullptr)
      magicStringLength = strlen(magicString);
    else
      magicStringLength = 0;
    if(mac != nullptr)
    {
      for (char i = 0; i < 6; ++i) 
        this->mac[i] = mac[i];
    }
    else
    {
      for (char i = 0; i < 6; ++i) 
        this->mac[i] = 0;
    }
    LOG_INFO("Создан класс DataTransmitter");
  }
#else
  DataTransmitter::DataTransmitter(const byte* mac, IPAddress ip, unsigned int port, const char* magicString) 
    : targetIP(255, 255, 255, 255), ip(ip), lockTargetIP(false)
    {
    this->port = port;
    this->magicString = magicString;
    if(magicString != nullptr)
      magicStringLength = strlen(magicString);
    else
      magicStringLength = 0;
    if(mac != nullptr)
    {
      for (char i = 0; i < 6; ++i) 
        this->mac[i] = mac[i];
    }
    else
    {
      for (char i = 0; i < 6; ++i) 
        this->mac[i] = 0;
    }
    LOG_INFO("Создан класс DataTransmitter");
  }
#endif
  void DataTransmitter::setLockTargetIP(bool lock)
  {
    lockTargetIP = lock;
  }
  void DataTransmitter::setTargetIP(IPAddress targetIP, bool lockTargetIP = true)
  {
    this->targetIP = targetIP;
    setLockTargetIP(lockTargetIP);
  }

  bool DataTransmitter::isValid()
  {
    if(magicString == nullptr)
      return false;
    bool res = false;
    for(char i = 0; i < 6; ++i)
      res = res || mac[i];
    if(!res)
      return false;
    return true;
  }

  int DataTransmitter::init()
  {
    if(!isValid())
      return 1;
    SPI.begin();
#if defined SPI_CLOCK_DIV2
    SPI.setClockDivider(SPI_CLOCK_DIV2);
#endif
#if USE_DHCP
    if (Ethernet.begin(mac) == 0)
    {
      LOG_ERROR("Ошибка DHCP");
      return 1;
    }
#else
    Ethernet.begin(mac, ip);
#endif
    if (!Udp.begin(port))
    {
      LOG_ERROR("Ошибка открытия UDP-порта");
      return 1;
    }
    return 0;
  }

  int DataTransmitter::sendData(const byte* data, int dataSize)
  {
    if(data == nullptr)
      return 1;
    Udp.beginPacket(targetIP, port);
    Udp.write(magicString, magicStringLength);
    Udp.write(data, dataSize);
    Udp.endPacket();
    LOG_INFO("Пакет отправлен");
    return 0;
  }
  int DataTransmitter::sendData(const char* data)
  {
    return sendData((const byte*)data, strlen(data)+1);
  }

  size_t DataTransmitter::receiveData(byte* buffer, int maxSize)
  {
    LOG_VERBOSE("Попытка получения пакета");
    if(buffer == nullptr)
    return 0;
    int packetSize = Udp.parsePacket();
    if ((packetSize < 1) || packetSize < magicStringLength)
    {
      LOG_VERBOSE("Некоректный размер пакета или его отсутствие");
      return 0;
    }
    LOG_VERBOSE("IP отправителя:");
    LOG_VERBOSE(Udp.remoteIP());
    if(Udp.remoteIP() == Ethernet.localIP())
    {
      LOG_INFO("Получен свой же пакет - игнорирование пакета");
      return 0;
    }
    if(packetSize > maxSize)
    {
      LOG_WARNING("Пакет проигнорирован, из-за того, что слишком большой");
      return 0;
    }
    Udp.read(buffer, packetSize);
    if (strncmp((char*)buffer, magicString, magicStringLength) != 0)
    {
      LOG_WARNING("В пакете нет магической строки");
      return 0;
    }
#if DEBUG_LEVEL >= DEBUG_LEVEL_INFO
    if(targetIP != Udp.remoteIP() && !lockTargetIP)
      LOG_INFO("IP аддресс удаленного устройства обновлен");
#endif
    if(!lockTargetIP)
      targetIP = Udp.remoteIP();
    memmove(buffer, &buffer[magicStringLength], packetSize - magicStringLength);
    LOG_INFO("Пакет получен");
    return packetSize - magicStringLength;
  }

  IPAddress DataTransmitter::getTargetIP()
  {
    return targetIP;
  }

  IPAddress DataTransmitter::getIP()
  {
    return Ethernet.localIP();
  }

#if !USE_DHCP
  void DataTransmitter::maintain() //Обновление DHCP
  {
    LOG_INFO("Обновление DHCP");
    Ethernet.maintain();
  }
#endif
