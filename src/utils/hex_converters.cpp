#include "hex_converters.h"

uint16_t hex_to_uint16(const char* hexString) {
  return (uint16_t)strtol(hexString, nullptr, HEX);
}

uint16_t hex_to_uint16(const String& hexString) {
  return hex_to_uint16(hexString.c_str());
}

uint8_t hex_to_uint8(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return 0;
}
