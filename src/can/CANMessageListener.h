#pragma once

#include <Arduino.h>
#include <Stream.h>

#include "../utils/hex_converters.h"
#include "CANMessageFilters.h"
#include "CANMessageProcessor.h"

class CANMessageListener {
 public:
  CANMessageListener() {}

  bool isListening() { return _listening && _stream; }

  CANMessageFilters* getFilters() { return _filters; }

  CANMessageProcessor* getProcessor() { return _processor; }
  void setProcessor(CANMessageProcessor* processor) { _processor = processor; }

  void startListening(Stream* stream, CANMessageFilters* filters) {
    _stream = stream;
    _filters = filters;
    _listening = true;
  }

  void stopListening() {
    _listening = false;
    _stream = nullptr;
    _filters = nullptr;
    clearBuffer();
  }

  void clearBuffer() { _buffer.clear(); }

  void update() {
    if (!isListening()) return;
    String raw = _buffer;
    int lastIndexOfCR = -1;
    while (isListening()) {
      raw += _stream->readString();
      // If we get a response with a newline, we're done
      lastIndexOfCR = raw.lastIndexOf("\r");
      if (lastIndexOfCR != -1) {
        break;
      }
      vTaskDelay(pdMS_TO_TICKS(10));
    }
    if (!isListening() || lastIndexOfCR == -1 || raw.length() == 0) {
      return;
    }
    if (raw.length() > lastIndexOfCR + 1) {
      _buffer = raw.substring(lastIndexOfCR + 1);
    } else {
      _buffer.clear();
    }

    _readLines(raw);
  }

 private:
  void _processRawMessage(String rawMessage) {
    if (rawMessage.length() > 3) {
      uint16_t messageId =
          CANMessageEncoder::parseMessageId(rawMessage.substring(0, 3));
      if (_filters->filter(messageId)) {
        String messageData = rawMessage.substring(3);
        if (_processor) {
          _processor->process(messageId, messageData);
        }
      }
    }
  }

  void _readLines(String& content) {
    int startIndex = 0;
    int endIndex = 0;

    while ((endIndex = content.indexOf('\r', startIndex)) != -1) {
      String line = content.substring(startIndex, endIndex);
      _processRawMessage(line);
      startIndex = endIndex + 1;
    }
  }

 private:
  Stream* _stream;
  bool _listening = false;
  String _buffer;
  CANMessageFilters* _filters;
  CANMessageProcessor* _processor;
};
