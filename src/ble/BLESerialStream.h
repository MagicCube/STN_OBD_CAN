#pragma once

#include <NimBLEDevice.h>
#include <Stream.h>

#include <functional>
#include <vector>

class BLESerialStream : public Stream {
 public:
  BLESerialStream(BLERemoteCharacteristic* characteristic,
                  const size_t bufferSize = 4 * 1024)
      : _characteristic(characteristic),
        _buffer(bufferSize),
        _bufferSize(bufferSize),
        _head(0),
        _tail(0),
        _isFull(false) {}

  void subscribe() {
    if (_characteristic) {
      _characteristic->subscribe(
          true, [this](NimBLERemoteCharacteristic* pBLERemoteCharacteristic,
                       uint8_t* pData, size_t length,
                       bool isNotify) { onReceive(pData, length); });
    }
  }

  String readString() override {
    size_t availableBytes = available();
    if (availableBytes == 0) {
      return String();
    }

    String result;
    result.reserve(availableBytes);

    size_t head = _head;
    size_t tail = _tail;
    bool isFull = _isFull;

    if (head >= tail) {
      // Buffer is not wrapped
      result.concat((char*)(&_buffer[tail]), availableBytes);
    } else {
      // Buffer is wrapped
      size_t firstPartSize = _bufferSize - tail;
      result.concat((char*)(&_buffer[tail]), firstPartSize);
      result.concat((char*)(&_buffer[0]), head);
    }

    // Clear buffer
    clearBuffer();

    return result;
  }

  /* String readAvailable() {
    size_t availableBytes = available();
    if (availableBytes == 0) {
      return String();
    }

    String result;
    result.reserve(availableBytes);

    while (availableBytes--) {
      int byte = read();
      if (byte == -1) {
        break;  // 处理 read() 返回 -1 的情况
      }
      result.concat((char)byte);
    }

    return result;
  } */

  void clearBuffer() {
    _head = 0;
    _tail = 0;
    _isFull = false;
  }

 protected:
  void onReceive(uint8_t* data, size_t length) {
    size_t head = _head;
    size_t tail = _tail;
    bool isFull = _isFull;

    for (size_t i = 0; i < length; ++i) {
      _buffer[head] = data[i];
      if (isFull) {
        tail = (tail + 1) % _bufferSize;
      }
      head = (head + 1) % _bufferSize;
      isFull = head == tail;
    }

    _head = head;
    _tail = tail;
    _isFull = isFull;
  }

  int available() override {
    if (_isFull) {
      return _bufferSize;
    }
    if (_head >= _tail) {
      return _head - _tail;
    }
    return _bufferSize - (_tail - _head);
  }

  int read() override {
    if (_head == _tail && !_isFull) {
      return -1;
    }
    uint8_t byte = _buffer[_tail];
    _tail = (_tail + 1) % _bufferSize;
    _isFull = false;
    return byte;
  }

  int peek() override {
    if (_head == _tail && !_isFull) {
      return -1;
    }
    return _buffer[_tail];
  }

  void flush() override { clearBuffer(); }

  size_t write(uint8_t byte) override {
    if (_characteristic) {
      _characteristic->writeValue(&byte, 1);
      return 1;
    }
    return 0;
  }

  size_t write(const uint8_t* buffer, size_t size) override {
    if (_characteristic) {
      _characteristic->writeValue(buffer, size);
      return size;
    }
    return 0;
  }

 private:
  BLERemoteCharacteristic* _characteristic;
  std::vector<uint8_t> _buffer;
  size_t _bufferSize;
  size_t _head;
  size_t _tail;
  bool _isFull;
};
