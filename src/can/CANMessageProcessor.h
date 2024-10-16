#pragma once

#include <Arduino.h>

class CANMessageProcessor {
 public:
  virtual ~CANMessageProcessor() = default;

  virtual void process(uint16_t messageId, const uint8_t* frame) = 0;
};
