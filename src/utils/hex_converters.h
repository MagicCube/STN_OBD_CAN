#pragma once

#include <Arduino.h>

uint16_t hex_to_uint16(const char* hexString);

uint16_t hex_to_uint16(const String& hexString);

uint8_t hex_to_uint8(char c);
