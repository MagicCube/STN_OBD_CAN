#pragma once

#include <Stream.h>

#include <vector>

class NimBLERemoteCharacteristic;

class BLESerialStream : public Stream {
 public:
  BLESerialStream(NimBLERemoteCharacteristic* characteristic,
                  const size_t bufferSize = 4 * 1024);

  void subscribe();

  String readString() override;

  void clearBuffer();

 protected:
  void onReceive(const uint8_t* data, size_t length);

  int available() override;

  int read() override;

  int peek() override;

  void flush() override;

  size_t write(uint8_t byte) override;

  size_t write(const uint8_t* buffer, size_t size) override;

 private:
  NimBLERemoteCharacteristic* _characteristic;
  std::vector<uint8_t> _buffer;
  const size_t _bufferSize;
  size_t _head;
  size_t _tail;
  bool _isFull;
};
