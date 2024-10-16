#pragma once

#include <Arduino.h>
#include <Stream.h>

class CANMessageDefinition;
class CANMessageDefinitionCollection;
class CANMessageProcessor;

class CANMessageListener {
 public:
  CANMessageListener(const CANMessageDefinitionCollection& messageDefinitions)
      : _messageDefinitions(messageDefinitions) {}

  bool isListening() const { return _listening && _stream; }

  CANMessageProcessor* getProcessor() const { return _processor; }
  void setProcessor(CANMessageProcessor* processor) { _processor = processor; }

  void startListening(Stream* stream);

  void stopListening();

  void clearBuffer();

  void update();

 private:
  void _processRawMessage(const CANMessageDefinition* messageDefinition,
                          const String& rawFrame);

  const int _readLines(const String& content);

 private:
  Stream* _stream;
  bool _listening = false;
  String _buffer;
  CANMessageProcessor* _processor;
  const CANMessageDefinitionCollection& _messageDefinitions;
};
