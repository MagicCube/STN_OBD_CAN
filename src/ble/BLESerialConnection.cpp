#include "BLESerialConnection.h"

#include <NimBLEDevice.h>

#include "BLESerialStream.h"

const NimBLEUUID SERIAL_SERVICE_ID("e7810a71-73ae-499d-8c15-faa9aef0c3f2");
const NimBLEUUID SERIAL_CHARACTERISTIC_ID(
    "bef8d6c9-9c21-4c9e-b632-bd58c1009f9f");

bool BLESerialConnection::connect(const char* deviceAddressLiteral) {
  NimBLEDevice::init("");
  NimBLEDevice::setPower(esp_power_level_t::ESP_PWR_LVL_P9);
  log_i("Connecting to BLE device...");
  NimBLEAddress deviceAddress(deviceAddressLiteral, 1);
  _client = NimBLEDevice::createClient(deviceAddress);
  if (!_client->connect()) {
    log_e("Failed to connect to BLE device");
    return false;
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
          log_i("handler %d", _characteristic->getHandle());
          _connected = true;
          log_i("BLE serial connection is now ready to interact");
          return true;
        }
      }
    }
  }
  log_e("BLE serial service not found");
  return false;
}

void BLESerialConnection::disconnect() {
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

BLESerialStream* BLESerialConnection::getStream() {
  if (!_stream && _characteristic) {
    _stream = new BLESerialStream(_characteristic);
    _stream->subscribe();
  }
  return _stream;
}
