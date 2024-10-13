#pragma once

#include <Arduino.h>

#include "../utils/hex_converters.h"
#include "../utils/unit_converters.h"
#include "CANMessageEncoder.h"

class CANMessageProcessor {
 public:
  virtual ~CANMessageProcessor() = default;

  virtual void process(uint16_t messageId, uint8_t* frame) = 0;
};
