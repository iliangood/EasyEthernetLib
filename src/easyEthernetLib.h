#if !defined EASY_ETHERNET_LIB
#define EASY_ETHERNET_LIB

#include "utils.h"

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#if !defined USE_DHCP
#define USE_DHCP false
#endif

class DataTransmitter {
private:
  uint8_t mac[6];
  IPAddress targetIP;
  bool lockTargetIP;
  EthernetUDP Udp;
  const char* magicString;
  size_t magicStringLength;
  unsigned int port;
#if !USE_DHCP
  IPAddress ip;
#endif
public:
#if USE_DHCP
  DataTransmitter(const byte* mac, unsigned int port, const char* magicString);
#else
  DataTransmitter(const byte* mac, unsigned int port, IPAddress ip, const char* magicString);
#endif
  void setLockTargetIP(bool lock);
  void setTargetIP(IPAddress targetIP, bool lockTargetIP = true);

  bool isValid();

  int init();

  int sendData(const byte* data, int dataSize);

  int sendData(const char* data);

  size_t receiveData(byte* buffer, int maxSize);

  IPAddress getTargetIP();

  IPAddress getIP();

#if not USE_DHCP
#if !USE_DHCP
  void maintain(); //Обновление DHCP
#endif
};

#endif
