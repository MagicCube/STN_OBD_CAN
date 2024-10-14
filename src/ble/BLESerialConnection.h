#pragma once

#include <NimBLEDevice.h>

class BLESerialStream;

class BLESerialConnection {
 public:
  BLESerialConnection();

  bool isConnected() const { return _connected; }

  bool connect(const char* deviceAddressLiteral);

  void disconnect();

  BLESerialStream* getStream();

 private:
  NimBLEClient* _client = nullptr;
  BLERemoteCharacteristic* _characteristic = nullptr;
  BLESerialStream* _stream = nullptr;
  bool _connected = false;
};
