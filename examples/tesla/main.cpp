#include <Arduino.h>
#include <STN_OBD_CAN.h>

#include "../examples/tesla/TeslaCANMessageProcessor.h"
#include "../examples/tesla/TeslaVehicle.h"

const char* DEVICE_ADDRESS = "c0:08:e1:98:fc:c8";

unsigned long monitoringStart = 0;
unsigned long lastTick = 0;
unsigned long tickCount = 0;
unsigned long tickInterval = 3 * 1000;

const uint16_t MESSAGE_IDS[] = {
    0x102,  // Left Door Status
    0x103,  // Right Door Status
    0x118,  // Drive System Status
    0x257,  // Speed
    0x273,  // Ambient Brightness
    0x292,  // State of Charge
    0x318,  // TODO: System Time UTC
    0x321,  // Temperature
    0x33A,  // Range
    0x3C2,  // TODO: Buttons
    0x3E2,  // Left Turn Signals / Brake Lights
    0x3E3,  // Right Turn Signals
};

TeslaVehicle vehicle;
OBDConnector connector;
CANMessageFilters filters(MESSAGE_IDS,
                          sizeof(MESSAGE_IDS) / sizeof(MESSAGE_IDS[0]));
CANMessageListener listener;
TeslaCANMessageProcessor processor(&vehicle);

void setup() {
  Serial.begin(115200);

  listener.setProcessor(&processor);
  connector.setFilters(&filters);
  connector.setListener(&listener);
  if (connector.connect(DEVICE_ADDRESS)) {
    connector.startMonitoring();
    monitoringStart = millis();
  }
}

void tick() {
  if (millis() - lastTick > tickInterval) {
    Serial.print(".");
    lastTick = millis();
    tickCount++;
    if (tickCount % (60 / (tickInterval / 1000)) == 0) {
      Serial.print(" [");
      Serial.print((millis() - monitoringStart) / 1000 / 60 + 1);
      Serial.print(" min] ");
      Serial.println(esp_get_free_heap_size());
    }
  }
}

void trackMemory() {
  if (millis() - lastTick > tickInterval) {
    Serial.println(esp_get_free_heap_size());
    lastTick = millis();
    tickCount++;
  }
}

void loop() {
  connector.update();
  if (listener.isListening()) {
    trackMemory();
  }
}
