#pragma once

#include <Arduino.h>

class CANMessageEncoder {
 public:
  static uint8_t* parseFrame(const String& hexString);

  static uint16_t parseMessageId(const String& hexString);

  static int8_t extract_int8(const uint8_t* frame, int startBit, int bitLength,
                             float factor = 1.0, float offset = 0.0);

  static uint8_t extract_uint8(const uint8_t* frame, int startBit,
                               int bitLength, float factor = 1.0,
                               float offset = 0.0);

  static int16_t extract_int16(const uint8_t* frame, int startBit,
                               int bitLength, float factor = 1.0,
                               float offset = 0.0);

  static uint16_t extract_uint16(const uint8_t* frame, int startBit,
                                 int bitLength, float factor = 1.0,
                                 float offset = 0.0);

  static int32_t extract_int32(const uint8_t* frame, int startBit,
                               int bitLength, float factor = 1.0,
                               float offset = 0.0);

  static uint32_t extract_uint32(const uint8_t* frame, int startBit,
                                 int bitLength, float factor = 1.0,
                                 float offset = 0.0);

  static float extract_float(const uint8_t* frame, int startBit, int bitLength,
                             float factor = 1.0, float offset = 0.0);

  static time_t extract_time(const uint8_t* frame);
};
