#pragma once

class BLESerialStream;
class NimBLEClient;
class NimBLERemoteCharacteristic;

class BLESerialConnection {
 public:
  bool isConnected() const { return _connected; }

  bool connect(const char* deviceAddressLiteral);

  void disconnect();

  BLESerialStream* getStream();

 private:
  NimBLEClient* _client = nullptr;
  NimBLERemoteCharacteristic* _characteristic = nullptr;
  BLESerialStream* _stream = nullptr;
  bool _connected = false;
};
