#pragma once

#include <NimBLEDevice.h>

#include "BLESerialStream.h"

const NimBLEUUID SERIAL_SERVICE_ID("e7810a71-73ae-499d-8c15-faa9aef0c3f2");
const NimBLEUUID SERIAL_CHARACTERISTIC_ID(
    "bef8d6c9-9c21-4c9e-b632-bd58c1009f9f");

class BLESerialConnection {
 public:
  BLESerialConnection() {}

  bool isConnected() const { return _connected; }

  bool connect(const char* deviceAddressLiteral) {
    NimBLEAddress deviceAddress(deviceAddressLiteral);
    NimBLEDevice::init("");
    NimBLEDevice::setPower(esp_power_level_t::ESP_PWR_LVL_P9);
    NimBLEScan* scan = NimBLEDevice::getScan();
    scan->setActiveScan(true);
    scan->setInterval(45);
    scan->setWindow(15);
    log_i("Scanning BLE devices for '%s'...", deviceAddressLiteral);
    auto results = scan->start(3);
    for (int i = 0; i < results.getCount(); i++) {
      NimBLEAdvertisedDevice advertisedDevice = results.getDevice(i);
      if (advertisedDevice.getAddress().equals(deviceAddress)) {
        log_i("BLE device found");
        onDeviceFound(&advertisedDevice);
        return isConnected();
      }
    }
    log_e("BLE device not found");
    return false;
  }

  void disconnect() {
    if (_client) {
      _client->disconnect();
      _client = nullptr;
    }
    if (_stream) {
      delete _stream;
      _stream = nullptr;
    }
    _connected = false;
  }

  BLESerialStream* getStream() {
    if (!_stream && _characteristic) {
      _stream = new BLESerialStream(_characteristic);
      _stream->subscribe();
    }
    return _stream;
  }

 protected:
  void onDeviceFound(NimBLEAdvertisedDevice* advertisedDevice) {
    log_i("Connecting to BLE device...");
    _client = NimBLEDevice::createClient();
    if (!_client->connect(advertisedDevice)) {
      log_e("Failed to connect to BLE device");
      return;
    }
    log_i("Connected to BLE device");

    auto services = _client->getServices(true);
    for (int i = 0; i < services->size(); i++) {
      auto service = services->at(i);
      if (service->getUUID().equals(SERIAL_SERVICE_ID)) {
        log_i("BLE serial service found");
        auto characteristics = service->getCharacteristics(true);
        for (int j = 0; j < characteristics->size(); j++) {
          auto characteristic = characteristics->at(j);
          if (characteristic->getUUID().equals(SERIAL_CHARACTERISTIC_ID)) {
            log_i("BLE serial characteristic found");
            _characteristic = characteristic;
            _connected = true;
            log_i("BLE serial connection is now ready to interact");
            return;
          }
        }
      }
    }
    log_e("BLE serial service not found");
  }

 private:
  NimBLEClient* _client = nullptr;
  BLERemoteCharacteristic* _characteristic = nullptr;
  BLESerialStream* _stream = nullptr;
  bool _connected = false;
};
