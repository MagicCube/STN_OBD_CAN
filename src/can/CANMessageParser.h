#pragma once

#include <Arduino.h>

#include "../utils/hex_converters.h"

class CANMessageParser {
 public:
  static uint8_t* parseFrame(const String& hexString) {
    int len = hexString.length();
    if (len % 2 != 0) return nullptr;
    int byteLen = len / 2;
    uint8_t* byteArray = new uint8_t[byteLen];
    for (int i = 0; i < byteLen; i++) {
      byteArray[i] = (hex_to_uint8(hexString[2 * i]) << 4) |
                     hex_to_uint8(hexString[2 * i + 1]);
    }
    return byteArray;
  }

  static uint16_t parseMessageId(const String& hexString) {
    return hex_to_uint16(hexString);
  }

  static String formatMessageId(uint16_t messageId) {
    auto hex = String(messageId, HEX);
    hex.toUpperCase();
    if (hex.length() == 1) {
      return "00" + hex;
    } else if (hex.length() == 2) {
      return "0" + hex;
    } else {
      return hex;
    }
  }

  static int8_t extract_int8(uint8_t* frame, int startBit, int bitLength,
                             float factor = 1.0, float offset = 0.0) {
    uint8_t mask = (1 << bitLength) - 1;
    int byteIndex = startBit / 8;
    int bitIndex = startBit % 8;
    int8_t value = (frame[byteIndex] >> bitIndex) & mask;
    if (value & (1 << (bitLength - 1))) {  // Check if the sign bit is set
      value |= ~mask;                      // Extend the sign bit
    }
    return static_cast<int8_t>(value * factor + offset);
  }

  static uint8_t extract_uint8(uint8_t* frame, int startBit, int bitLength,
                               float factor = 1.0, float offset = 0.0) {
    uint8_t mask = (1 << bitLength) - 1;
    int byteIndex = startBit / 8;
    int bitIndex = startBit % 8;
    uint8_t value = (frame[byteIndex] >> bitIndex) & mask;
    return static_cast<uint8_t>(value * factor + offset);
  }

  static int16_t extract_int16(uint8_t* frame, int startBit, int bitLength,
                               float factor = 1.0, float offset = 0.0) {
    uint16_t mask = (1 << bitLength) - 1;
    int byteIndex = startBit / 8;
    int bitIndex = startBit % 8;
    int16_t value =
        (frame[byteIndex] | (frame[byteIndex + 1] << 8)) >> bitIndex;
    value &= mask;
    if (value & (1 << (bitLength - 1))) {  // Check if the sign bit is set
      value |= ~mask;                      // Extend the sign bit
    }
    return static_cast<int16_t>(value * factor + offset);
  }

  static uint16_t extract_uint16(uint8_t* frame, int startBit, int bitLength,
                                 float factor = 1.0, float offset = 0.0) {
    uint16_t mask = (1 << bitLength) - 1;
    int byteIndex = startBit / 8;
    int bitIndex = startBit % 8;
    uint16_t value =
        (frame[byteIndex] | (frame[byteIndex + 1] << 8)) >> bitIndex;
    value &= mask;
    return static_cast<uint16_t>(value * factor + offset);
  }

  static int32_t extract_int32(uint8_t* frame, int startBit, int bitLength,
                               float factor = 1.0, float offset = 0.0) {
    uint32_t mask = (1UL << bitLength) - 1;
    int byteIndex = startBit / 8;
    int bitIndex = startBit % 8;
    int32_t value =
        (frame[byteIndex] | (frame[byteIndex + 1] << 8) |
         (frame[byteIndex + 2] << 16) | (frame[byteIndex + 3] << 24)) >>
        bitIndex;
    value &= mask;
    if (value & (1 << (bitLength - 1))) {  // Check if the sign bit is set
      value |= ~mask;                      // Extend the sign bit
    }
    return static_cast<int32_t>(value * factor + offset);
  }

  static uint32_t extract_uint32(uint8_t* frame, int startBit, int bitLength,
                                 float factor = 1.0, float offset = 0.0) {
    uint32_t mask = (1UL << bitLength) - 1;
    int byteIndex = startBit / 8;
    int bitIndex = startBit % 8;
    uint32_t value =
        (frame[byteIndex] | (frame[byteIndex + 1] << 8) |
         (frame[byteIndex + 2] << 16) | (frame[byteIndex + 3] << 24)) >>
        bitIndex;
    value &= mask;
    return static_cast<uint32_t>(value * factor + offset);
  }

  static float extract_float(uint8_t* frame, int startBit, int bitLength,
                             float factor = 1.0, float offset = 0.0) {
    uint32_t value = extract_uint32(frame, startBit, bitLength);
    return static_cast<float>(value * factor + offset);
  }

  static time_t extract_time(uint8_t* frame) {
    uint16_t year = extract_uint8(frame, 0, 8) + 2000;
    uint8_t month = extract_uint8(frame, 8, 8);
    uint8_t seconds = extract_uint8(frame, 16, 8);
    uint8_t hour = extract_uint8(frame, 24, 8);
    uint8_t day = extract_uint8(frame, 32, 8);
    uint8_t minutes = extract_uint8(frame, 40, 8);
    tm timeInfo;
    timeInfo.tm_year = year - 1900;
    timeInfo.tm_mon = month - 1;
    timeInfo.tm_mday = day;
    timeInfo.tm_hour = hour;
    timeInfo.tm_min = minutes;
    timeInfo.tm_sec = seconds;
    return mktime(&timeInfo);
  }
};
