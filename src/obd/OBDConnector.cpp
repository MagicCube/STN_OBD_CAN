#include "OBDConnector.h"

#include "../ble/BLESerialConnection.h"
#include "../ble/BLESerialStream.h"
#include "../can/CANMessageDefinition.h"
#include "../can/CANMessageDefinitionCollection.h"
#include "../can/CANMessageListener.h"

OBDConnector::OBDConnector(
    BLESerialConnection& connection,
    const CANMessageDefinitionCollection& messageDefinitions)
    : _connection(connection), _messageDefinitions(messageDefinitions) {}

bool OBDConnector::isConnected() const {
  return _connection.isConnected() && _stream;
}

bool OBDConnector::isMonitoring() const { return isConnected() && _monitoring; }

bool OBDConnector::connect(const char* deviceAddress) {
  bool connected = _connection.connect(deviceAddress);
  if (connected) {
    _stream = _connection.getStream();
    resetOBD();
  }
  return connected;
}

void OBDConnector::resetOBD() {
  if (!isConnected()) return;

  log_i("Initializing OBD...");
  sendCommand("AT D");     // Set all to defaults
  sendCommand("AT E0");    // Disable echo
  sendCommand("AT L0");    // Disable linefeeds
  sendCommand("AT H1");    // Turn on header
  sendCommand("AT S0");    // Turn off space
  sendCommand("AT SP 6");  // Set protocol to ISO 15765
  sendCommand("AT CAF0");  // Turn off CAN auto-flow control
  sendCommand("AT CFC0");  // Turn off CAN flow control
  auto messageIds = _messageDefinitions.getMessageIds();
  for (const auto& messageId : messageIds) {
    auto command = String("ST FAP ") + messageId + ",7FF";
    sendCommand(command);
  }
  log_i("OBD is now ready to monitor.");
}

void OBDConnector::sendCommand(const char* command, bool expectOK) {
  if (!isConnected()) return;

  tx(command);
  if (expectOK) {
    String response = "";
    do {
      response = rx();
    } while (response && response.indexOf("OK") == -1);
  }
}

void OBDConnector::sendCommand(const String& command, bool expectOK) {
  sendCommand(command.c_str(), expectOK);
}

void OBDConnector::startMonitoring() {
  if (!_connection.isConnected()) return;
  if (_monitoring) return;
  sendCommand("ST M", false);
  if (_listener) {
    _listener->startListening(_stream);
  }
  log_i("Monitoring is now started");
  _monitoring = true;
}

void OBDConnector::stopMonitoring() {
  if (!isMonitoring()) return;
  if (_listener) {
    _listener->stopListening();
  }
  // Press any key to stop
  sendCommand("AT", false);
  if (_stream) {
    _stream->clearBuffer();
  }
  log_i("Monitoring is now stopped");
  _monitoring = false;
}

void OBDConnector::update() {
  if (isMonitoring()) {
    if (_listener) {
      _listener->update();
    }
  }
}

String OBDConnector::rx() {
  String response = _stream->readStringUntil('>');
#if OBD_CONNECTOR_LOG_ENABLED
  if (response && response.length() > 0) {
    Serial.print("RX: ");
    Serial.println(response);
  }
#endif
  return response;
}

void OBDConnector::tx(const String& command) {
#if OBD_CONNECTOR_LOG_ENABLED
  Serial.print("TX: ");
  Serial.println(command);
#endif
  _stream->print(command);
  _stream->print("\r");
}
