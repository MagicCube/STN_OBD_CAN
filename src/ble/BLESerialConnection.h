#pragma once

#include <Arduino.h>

class BLESerialStream;
class NimBLEClient;
class NimBLERemoteCharacteristic;

class BLESerialConnection {
 public:
  BLESerialConnection(uint16_t bufferSize = 1 * 1024)
      : _bufferSize(bufferSize) {}

  bool isConnected() const { return _connected; }

  bool connect(const char* deviceAddressLiteral);

  void disconnect();

  BLESerialStream* getStream();

 private:
  uint16_t _bufferSize;
  NimBLEClient* _client = nullptr;
  NimBLERemoteCharacteristic* _characteristic = nullptr;
  BLESerialStream* _stream = nullptr;
  bool _connected = false;
};
