#pragma once

class CANMessageDefinition {
 private:
  uint16_t _id;
  uint8_t _length;

 public:
  CANMessageDefinition() = default;

  // Constructor to initialize the CAN message definition
  CANMessageDefinition(uint16_t id, uint8_t length = 8)
      : _id(id), _length(length) {}

  // Get message id
  uint16_t id() const { return _id; }

  // Get message ID in hex
  String hexId() const { return _formatMessageId(_id); }

  // Get message length in byte
  uint8_t length() const { return _length; }

 private:
  static String _formatMessageId(uint16_t messageId) {
    auto hex = String(messageId, HEX);
    hex.toUpperCase();
    if (hex.length() == 1) {
      return "00" + hex;
    } else if (hex.length() == 2) {
      return "0" + hex;
    }
    return hex;
  }
};
