#pragma once

#include <Arduino.h>

#include <vector>

// TODO: 使用 hash 表优化过滤器
class CANMessageFilters {
 public:
  CANMessageFilters(const uint16_t passMessageIds[], size_t count) {
    _passMessageIds =
        std::vector<uint16_t>(passMessageIds, passMessageIds + count);
  }

  std::vector<uint16_t>& getPassMessageIds() { return _passMessageIds; }

  bool filter(uint16_t messageId) {
    return std::find(_passMessageIds.begin(), _passMessageIds.end(),
                     messageId) != _passMessageIds.end();
  }

  void clear() { _passMessageIds.clear(); };

 private:
  std::vector<uint16_t> _passMessageIds;
};
