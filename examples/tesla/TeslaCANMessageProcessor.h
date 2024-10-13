#pragma once

#include <Arduino.h>
#include <STN_OBD_CAN.h>

#include "TeslaVehicle.h"

class TeslaCANMessageProcessor : public CANMessageProcessor {
 public:
  TeslaCANMessageProcessor(TeslaVehicle* vehicle) {}

  TeslaVehicle* getVehicle() { return _vehicle; }

  void process(uint16_t messageId, String& messageData) override {
    switch (messageId) {
      case 0x102: {
        if (messageData.length() == 16) {
          auto frame = CANMessageParser::parseFrame(messageData);
          if (frame) {
            auto value = CANMessageParser::extract_uint8(frame, 0, 4);
            _vehicle->setFrontLeftDoorClosed(value == 2);
            value = CANMessageParser::extract_uint8(frame, 4, 4);
            _vehicle->setRearLeftDoorClosed(value == 2);
            delete[] frame;
          }
        }
        break;
      }
      case 0x103: {
        if (messageData.length() == 16) {
          auto frame = CANMessageParser::parseFrame(messageData);
          if (frame) {
            auto value = CANMessageParser::extract_uint8(frame, 0, 4);
            _vehicle->setFrontRightDoorClosed(value == 2);
            value = CANMessageParser::extract_uint8(frame, 4, 4);
            _vehicle->setRearRightDoorClosed(value == 2);
            delete[] frame;
          }
        }
        break;
      }
      case 0x118: {
        if (messageData.length() == 16) {
          auto frame = CANMessageParser::parseFrame(messageData);
          if (frame) {
            auto value = CANMessageParser::extract_uint8(frame, 21, 3);
            if (value == 0 || value == 7) {
              _vehicle->setGear(Gear::UNKNOWN);
            } else {
              _vehicle->setGear(static_cast<Gear>(value));
            }
            delete[] frame;
          }
        }
        break;
      }
      case 0x257: {
        if (messageData.length() == 16) {
          auto frame = CANMessageParser::parseFrame(messageData);
          if (frame) {
            auto value = CANMessageParser::extract_uint16(frame, 24, 9);
            _vehicle->setSpeed(value);
            delete[] frame;
          }
        }
        break;
      }
      case 0x273: {
        if (messageData.length() == 16) {
          auto frame = CANMessageParser::parseFrame(messageData);
          if (frame) {
            auto value = CANMessageParser::extract_uint8(frame, 32, 8, 0.5);
            _vehicle->setDisplayBrightnessLevel(value);
            delete[] frame;
          }
        }
        break;
      }
      case 0x292: {
        if (messageData.length() == 16) {
          auto frame = CANMessageParser::parseFrame(messageData);
          if (frame) {
            auto stateOfCharge =
                CANMessageParser::extract_float(frame, 0, 10, 0.1);
            _vehicle->setStateOfCharge(stateOfCharge);
            delete[] frame;
          }
        }
        break;
      }
      case 0x321: {
        if (messageData.length() == 16) {
          auto frame = CANMessageParser::parseFrame(messageData);
          if (frame) {
            auto value =
                CANMessageParser::extract_float(frame, 40, 8, 0.5, -40);
            _vehicle->setTemperatureAmbient(value);
            delete[] frame;
          }
        }
        break;
      }
      case 0x33a: {
        if (messageData.length() == 16) {
          auto frame = CANMessageParser::parseFrame(messageData);
          if (frame) {
            auto value = CANMessageParser::extract_uint16(frame, 0, 10);
            auto valueInKm = convertMilesToKilometers(value);
            _vehicle->setRange(valueInKm);
            delete[] frame;
          }
        }
        break;
      }
      case 0x3e2: {
        if (messageData.length() == 14) {
          auto frame = CANMessageParser::parseFrame(messageData);
          if (frame) {
            auto value = CANMessageParser::extract_uint8(frame, 4, 2);
            _vehicle->setLeftTurnSignal(value == 1);
            value = CANMessageParser::extract_uint8(frame, 0, 2);
            _vehicle->setBrakeLight(value == 1);
            delete[] frame;
          }
        }
        break;
      }
      case 0x3e3: {
        if (messageData.length() == 8) {
          auto frame = CANMessageParser::parseFrame(messageData);
          if (frame) {
            auto value = CANMessageParser::extract_uint8(frame, 4, 2);
            _vehicle->setRightTurnSignal(value == 1);
            delete[] frame;
          }
        }
        break;
      }
      default:
        break;
    }
  }

 private:
  TeslaVehicle* _vehicle;
};
