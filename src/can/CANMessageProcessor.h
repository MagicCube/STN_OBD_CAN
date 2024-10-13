#pragma once

#include <Arduino.h>

#include "../utils/hex_converters.h"
#include "../utils/unit_converters.h"
#include "CANMessageParser.h"

class CANMessageProcessor {
 public:
  virtual ~CANMessageProcessor() = default;

  virtual void process(uint16_t messageId, String& messageData) = 0;
};
