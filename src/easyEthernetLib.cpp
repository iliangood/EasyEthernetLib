#include "easyEthernetLib.h"

#include "utils.h"

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

  DataTransmitter::DataTransmitter(const uint8_t* mac, unsigned int port, const char* magicString) 
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
	  for (int i = 0; i < 6; ++i) 
		this->mac[i] = mac[i];
	}
	else
	{
	  for (int i = 0; i < 6; ++i) 
		this->mac[i] = 0;
	}
	DEBUG_VERBOSE("Создан класс DataTransmitter");
  }

  void DataTransmitter::setLockTargetIP(bool lock)
  {
	lockTargetIP = lock;
  }
  void DataTransmitter::setTargetIP(IPAddress targetIP, bool lockTargetIP)
  {
	this->targetIP = targetIP;
	setLockTargetIP(lockTargetIP);
  }

  bool DataTransmitter::isValid()
  {
	if(magicStringLength > 0 && magicString == nullptr)
	  return false;
	bool res = false;
	for(int i = 0; i < 6; ++i)
	  res = res || mac[i] != 0;
	if(!res)
	  return false;
	return true;
  }

  /*int DataTransmitter::init()
  {
	useDHCP = true;
	if(!isValid())
	  return 1;
	SPI.begin();
#if defined SPI_CLOCK_DIV2
	SPI.setClockDivider(SPI_CLOCK_DIV2);
#endif
	if (Ethernet.begin(mac) == 0)
	{
	  DEBUG_ERROR("Ошибка DHCP");
	  return 1;
	}
	if (!Udp.begin(port))
	{
	  DEBUG_ERROR("Ошибка открытия UDP-порта");
	  return 1;
	}
	return 0;
  }*/

  int DataTransmitter::init(IPAddress ip)
  {
	useDHCP = false;
	if(!isValid())
	{
		DEBUG_ERROR("Ошибка инициализации - невалидные параметры");
		return 1;
	}
	SPI.begin();
#if defined SPI_CLOCK_DIV2
	SPI.setClockDivider(SPI_CLOCK_DIV2);
#endif
	Ethernet.begin(mac, ip);
	if (!Udp.begin(port))
	{
	  DEBUG_ERROR("Ошибка открытия UDP-порта");
	  return 1;
	}
	return 0;
  }

  int DataTransmitter::sendData(const byte* data, size_t dataSize)
  {
	if(data == nullptr)
	  return 1;
	Udp.beginPacket(targetIP, port);
	if(magicStringLength > 0)
		Udp.write(magicString, magicStringLength);
	Udp.write(data, dataSize);
	Udp.endPacket();
	if(magicStringLength > 0)
	  DEBUG_DEBUG("Пакет отправлен с магической строкой");
	else
	  DEBUG_DEBUG("Пакет отправлен без магической строки");
	return 0;
  }
  int DataTransmitter::sendData(const char* data)
  {
	return sendData((const byte*)data, strlen(data)+1);
  }

  //#pragma GCC push_options
  //#pragma GCC optimize ("O0")

  receiveInfo __attribute__((used, noinline, optimize("O0"))) DataTransmitter::receiveData(byte* buffer, size_t maxSize)
  {
	DEBUG_VERBOSE("Попытка получения пакета");
	//Serial.println("Попытка получения пакета");
	if(buffer == nullptr)
	return receiveInfo{0, IPAddress(0,0,0,0)};
	size_t packetSize = max(Udp.parsePacket(), 0);
	if ((packetSize < 1) || packetSize < magicStringLength)
	{
	  Serial.println("Некоректный размер пакета или его отсутствие");
	  DEBUG_DEBUG("Некоректный размер пакета или его отсутствие");
	  return receiveInfo{0, IPAddress(0,0,0,0)};
	}
	
	DEBUG_VERBOSE("IP отправителя:%d.%d.%d.%d", Udp.remoteIP()[0], Udp.remoteIP()[1], Udp.remoteIP()[2], Udp.remoteIP()[3]);
	if(Udp.remoteIP() == Ethernet.localIP())
	{
	  DEBUG_DEBUG("Получен свой же пакет - игнорирование пакета");
	  Udp.flush();
	  return receiveInfo{0, Udp.remoteIP()};
	}
	if(lockTargetIP && targetIP != IPAddress(255, 255, 255, 255) && targetIP != Udp.remoteIP())
	{
	  DEBUG_WARNING("Пакет проигнорирован, из-за того, что пришел не с того IP");
	  Udp.flush();
	  return receiveInfo{0, Udp.remoteIP()};
	}
	if(packetSize > maxSize)
	{
	  DEBUG_WARNING("Пакет проигнорирован, из-за того, что слишком большой");
	  Udp.flush();
	  return receiveInfo{0, Udp.remoteIP()};
	}
	Udp.read(buffer, packetSize);
	/*for(size_t i = 0; i < packetSize; ++i)
	{
		Serial.print(buffer[i], HEX);
		Serial.print(' ');
	}
	Serial.print('\n');
	for(size_t i = 0; i < packetSize; ++i)
	{
		Serial.print(static_cast<char>(buffer[i]));
		Serial.print(' ');
	}*/
	//Serial.print('\n');
	if(magicStringLength > 0)
	{
		if (strncmp((char*)buffer, magicString, magicStringLength) != 0)
		{
			DEBUG_WARNING("В пакете нет ожидаемой магической строки");
			Udp.flush();
			return receiveInfo{0, Udp.remoteIP()};
		}
	}
	else
		DEBUG_DEBUG("Пакет получен без магической строки");
	if(targetIP != Udp.remoteIP() && !lockTargetIP)
	{
		DEBUG_INFO("IP аддресс удаленного устройства обновлен");
		targetIP = Udp.remoteIP();
	}
	memmove(buffer, &buffer[magicStringLength], packetSize - magicStringLength);
	DEBUG_INFO("Пакет получен");
	Udp.flush();
	return receiveInfo{(size_t)(packetSize - magicStringLength), Udp.remoteIP()};
  }

  //#pragma GCC pop_options

  IPAddress DataTransmitter::getTargetIP()
  {
	return targetIP;
  }

  IPAddress DataTransmitter::getIP()
  {
	return Ethernet.localIP();
  }


  int DataTransmitter::maintain() //Обновление DHCP
  {
	if(!useDHCP)
	{
	  DEBUG_ERROR("Использована функция для работы с DHCP с иницализацией без DHCP");
	  return 1;
	}
	DEBUG_INFO("Обновление DHCP");
	int rc = Ethernet.maintain();
	switch (rc) {
	case 0:
	  DEBUG_VERBOSE("DHCP: Ничего не произошло");
	  break;
	case 1:
	  DEBUG_ERROR("DHCP: Не удалось продлить аренду");
	  return 1;
	case 2:
	  DEBUG_INFO("DHCP: Аренда успешно продлена");
	  break;
	case 3:
	  DEBUG_ERROR("DHCP: Не удалось выполнить повторную привязку");
	  return 1;
	case 4:
	  DEBUG_INFO("DHCP: Повторная привязка успешна");
	  break;
  }
	return 0;
  }

