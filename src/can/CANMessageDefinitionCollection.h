#pragma once

#include <Arduino.h>

#include <unordered_map>
#include <vector>

#include "CANMessageDefinition.h"

class CANMessageDefinitionCollection {
 public:
  // Constructor to initialize the collection with multiple
  // CANMessageDefinitions
  CANMessageDefinitionCollection(
      std::initializer_list<CANMessageDefinition> definitions) {
    for (const auto& def : definitions) {
      _definitions[def.id()] = def;
    }
  }

  // Get the CANMessageDefinition by id
  const CANMessageDefinition* get(uint16_t messageId) const {
    auto it = _definitions.find(messageId);
    if (it != _definitions.end()) {
      return &(it->second);
    }
    return nullptr;  // Return nullptr if the id is not found
  }

  // Check if a definition with the given id exists
  bool has(uint16_t messageId) const {
    return _definitions.find(messageId) != _definitions.end();
  }

  // Get all ids as a vector of strings
  std::vector<String> getMessageIds() const {
    std::vector<String> ids;
    for (const auto& pair : _definitions) {
      auto definition = pair.second;
      ids.push_back(definition.hexId());
    }
    return ids;
  }

 private:
  std::unordered_map<uint16_t, CANMessageDefinition> _definitions;
};
