#include "CANMessageListener.h"

#include "../utils/hex_converters.h"
#include "CANMessageDefinition.h"
#include "CANMessageDefinitionCollection.h"
#include "CANMessageEncoder.h"
#include "CANMessageProcessor.h"

void CANMessageListener::startListening(Stream* stream) {
  _stream = stream;
  _listening = true;
}

void CANMessageListener::stopListening() {
  _listening = false;
  _stream = nullptr;
  clearBuffer();
}

void CANMessageListener::clearBuffer() { _buffer.clear(); }

void CANMessageListener::update() {
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

void CANMessageListener::_processRawMessage(
    const CANMessageDefinition* messageDefinition, const String& rawFrame) {
  if (messageDefinition->length() * 2 == rawFrame.length()) {
    auto frame = CANMessageEncoder::parseFrame(rawFrame);
    if (_processor) {
      _processor->process(messageDefinition->id(), frame);
    }
    delete[] frame;
  }
}

void CANMessageListener::_readLines(const String& content) {
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
