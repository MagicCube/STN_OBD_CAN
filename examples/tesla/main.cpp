#include <Arduino.h>
#include <STN_OBD_CAN.h>

#include "TeslaCANMessageProcessor.h"
#include "TeslaVehicle.h"
#include "definitions.h"

const char* DEVICE_ADDRESS = "c0:08:e1:98:fc:c8";

unsigned long monitoringStart = 0;

TeslaVehicle vehicle;

// In my case, setting the buffer size to 1KB works the best.
BLESerialConnection connection(1 * 1024);
TeslaCANMessageProcessor processor(vehicle);
CANMessageListener listener(TESLA_CAN_MESSAGES);
OBDConnector connector(connection, TESLA_CAN_MESSAGES);

void setup() {
  Serial.begin(115200);

  listener.setProcessor(&processor);
  connector.setListener(&listener);
  if (connector.connect(DEVICE_ADDRESS)) {
    connector.startMonitoring();
    monitoringStart = millis();
  }
}

unsigned long lastTick = 0;
unsigned long tickCount = 0;
unsigned long tickInterval = 3 * 1000;
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

long lastUpdateInLoop = 0;
void detectLongTask() {
  auto delta = millis() - lastUpdateInLoop;
  if (delta > 1000 / 60) {
    log_w("\nLong task detected: %dms\n", delta);
  }
  lastUpdateInLoop = millis();
}

void trackMemory() {
  if (millis() - lastTick > tickInterval) {
    Serial.print("Free heap: ");
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
  detectLongTask();
}
