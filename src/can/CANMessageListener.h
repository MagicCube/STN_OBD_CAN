#pragma once

#include <Arduino.h>
#include <Stream.h>

#include "../utils/hex_converters.h"
#include "CANMessageDefinition.h"
#include "CANMessageDefinitionCollection.h"
#include "CANMessageProcessor.h"

class CANMessageListener {
 public:
  CANMessageListener(const CANMessageDefinitionCollection& messageDefinitions)
      : _messageDefinitions(messageDefinitions) {}

  bool isListening() const { return _listening && _stream; }

  CANMessageProcessor* getProcessor() const { return _processor; }
  void setProcessor(CANMessageProcessor* processor) { _processor = processor; }

  void startListening(Stream* stream) {
    _stream = stream;
    _listening = true;
  }

  void stopListening() {
    _listening = false;
    _stream = nullptr;
    clearBuffer();
  }

  void clearBuffer() { _buffer.clear(); }

  void update() {
    if (!isListening()) return;

    _buffer += _stream->readString();

    int lastIndexOfCR = _buffer.lastIndexOf("\r");
    if (lastIndexOfCR == -1) {
      // If there's no complete message, we're done
      return;
    }

    _readLines(_buffer);

    // If there's a partial message left, keep it for next time,
    // otherwise clear the buffer
    if (_buffer.length() > lastIndexOfCR + 1) {
      _buffer = _buffer.substring(lastIndexOfCR + 1);
    } else {
      clearBuffer();
    }
  }

 private:
  void _processRawMessage(const CANMessageDefinition* messageDefinition,
                          const String& rawFrame) {
    if (messageDefinition->length() * 2 == rawFrame.length()) {
      auto frame = CANMessageEncoder::parseFrame(rawFrame);
      if (_processor) {
        _processor->process(messageDefinition->id(), frame);
      }
      delete[] frame;
    }
  }

  void _readLines(String& content) {
    int startIndex = 0;
    int endIndex = 0;

    while ((endIndex = content.indexOf('\r', startIndex)) != -1) {
      if (endIndex - startIndex > 3) {
        String rawMessageId = content.substring(startIndex, startIndex + 3);
        uint16_t messageId = CANMessageEncoder::parseMessageId(rawMessageId);
        auto messageDefinition = _messageDefinitions.get(messageId);
        if (messageDefinition) {
          String rawFrame = content.substring(startIndex + 3, endIndex);
          _processRawMessage(messageDefinition, rawFrame);
        }
      }
      startIndex = endIndex + 1;
    }
  }

 private:
  Stream* _stream;
  bool _listening = false;
  String _buffer;
  CANMessageProcessor* _processor;
  const CANMessageDefinitionCollection& _messageDefinitions;
};
