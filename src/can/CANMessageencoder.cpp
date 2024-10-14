#include "CANMessageEncoder.h"

#include "../utils/hex_converters.h"

uint8_t* CANMessageEncoder::parseFrame(const String& hexString) {
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

uint16_t CANMessageEncoder::parseMessageId(const String& hexString) {
  return hex_to_uint16(hexString);
}

int8_t CANMessageEncoder::extract_int8(const uint8_t* frame, int startBit,
                                       int bitLength, float factor,
                                       float offset) {
  uint8_t mask = (1 << bitLength) - 1;
  int byteIndex = startBit / 8;
  int bitIndex = startBit % 8;
  int8_t value = (frame[byteIndex] >> bitIndex) & mask;
  if (value & (1 << (bitLength - 1))) {  // Check if the sign bit is set
    value |= ~mask;                      // Extend the sign bit
  }
  return static_cast<int8_t>(value * factor + offset);
}

uint8_t CANMessageEncoder::extract_uint8(const uint8_t* frame, int startBit,
                                         int bitLength, float factor,
                                         float offset) {
  uint8_t mask = (1 << bitLength) - 1;
  int byteIndex = startBit / 8;
  int bitIndex = startBit % 8;
  uint8_t value = (frame[byteIndex] >> bitIndex) & mask;
  return static_cast<uint8_t>(value * factor + offset);
}

int16_t CANMessageEncoder::extract_int16(const uint8_t* frame, int startBit,
                                         int bitLength, float factor,
                                         float offset) {
  uint16_t mask = (1 << bitLength) - 1;
  int byteIndex = startBit / 8;
  int bitIndex = startBit % 8;
  int16_t value = (frame[byteIndex] | (frame[byteIndex + 1] << 8)) >> bitIndex;
  value &= mask;
  if (value & (1 << (bitLength - 1))) {  // Check if the sign bit is set
    value |= ~mask;                      // Extend the sign bit
  }
  return static_cast<int16_t>(value * factor + offset);
}

uint16_t CANMessageEncoder::extract_uint16(const uint8_t* frame, int startBit,
                                           int bitLength, float factor,
                                           float offset) {
  uint16_t mask = (1 << bitLength) - 1;
  int byteIndex = startBit / 8;
  int bitIndex = startBit % 8;
  uint16_t value = (frame[byteIndex] | (frame[byteIndex + 1] << 8)) >> bitIndex;
  value &= mask;
  return static_cast<uint16_t>(value * factor + offset);
}

int32_t CANMessageEncoder::extract_int32(const uint8_t* frame, int startBit,
                                         int bitLength, float factor,
                                         float offset) {
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

uint32_t CANMessageEncoder::extract_uint32(const uint8_t* frame, int startBit,
                                           int bitLength, float factor,
                                           float offset) {
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

float CANMessageEncoder::extract_float(const uint8_t* frame, int startBit,
                                       int bitLength, float factor,
                                       float offset) {
  uint32_t value = extract_uint32(frame, startBit, bitLength);
  return static_cast<float>(value * factor + offset);
}

time_t CANMessageEncoder::extract_time(const uint8_t* frame) {
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
