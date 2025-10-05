#include "easyEthernetLib.h"

#include "utils.h"

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

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
	DEBUG_VERBOSE("Создан класс DataTransmitter");
  }

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
	if(port == 0)
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

  int DataTransmitter::sendData(const byte* data, int dataSize)
  {
	if(data == nullptr)
	  return 1;
	Udp.beginPacket(targetIP, port);
	Udp.write(magicString, magicStringLength);
	Udp.write(data, dataSize);
	Udp.endPacket();
	DEBUG_INFO("Пакет отправлен");
	return 0;
  }
  int DataTransmitter::sendData(const char* data)
  {
	return sendData((const byte*)data, strlen(data)+1);
  }
  
  size_t DataTransmitter::receiveData(byte* buffer, int maxSize)
  {
	DEBUG_VERBOSE("Попытка получения пакета");
	if(buffer == nullptr)
	return 0;
	int packetSize = Udp.parsePacket();
	if ((packetSize < 1) || packetSize < magicStringLength)
	{
	  DEBUG_VERBOSE("Некоректный размер пакета или его отсутствие");
	  return 0;
	}
	DEBUG_VERBOSE("IP отправителя:%d.%d.%d.%d", Udp.remoteIP()[0], Udp.remoteIP()[1], Udp.remoteIP()[2], Udp.remoteIP()[3]);
	if(Udp.remoteIP() == Ethernet.localIP())
	{
	  DEBUG_INFO("Получен свой же пакет - игнорирование пакета");
	  Udp.flush();
	  return 0;
	}
	if(lockTargetIP && targetIP != IPAddress(255, 255, 255, 255) && targetIP != Udp.remoteIP())
	{
	  DEBUG_WARNING("Пакет проигнорирован, из-за того, что пришел не с того IP");
	  Udp.flush();
	  return 0;
	}
	if(packetSize > maxSize)
	{
	  DEBUG_WARNING("Пакет проигнорирован, из-за того, что слишком большой");
	  Udp.flush();
	  return 0;
	}
	Udp.read(buffer, packetSize);
	if (strncmp((char*)buffer, magicString, magicStringLength) != 0)
	{
	  DEBUG_WARNING("В пакете нет магической строки");
	  Udp.flush();
	  return 0;
	}
	if(targetIP != Udp.remoteIP() && !lockTargetIP)
	{
		DEBUG_INFO("IP аддресс удаленного устройства обновлен");
		targetIP = Udp.remoteIP();
	}
	memmove(buffer, &buffer[magicStringLength], packetSize - magicStringLength);
	DEBUG_INFO("Пакет получен");
	Udp.flush();
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
	  DEBUG_INFO("DHCP: Ничего не произошло");
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

