#pragma once

#include <Arduino.h>

class BLESerialConnection;
class BLESerialStream;
class CANMessageDefinitionCollection;
class CANMessageListener;

#define OBD_CONNECTOR_LOG_ENABLED 0

class OBDConnector {
 public:
  OBDConnector(BLESerialConnection& connection,
               const CANMessageDefinitionCollection& messageDefinitions);

  bool isConnected() const;

  bool isMonitoring() const;

  bool connect(const char* deviceAddress);

  void resetOBD();

  CANMessageListener* getListener() const { return _listener; }
  void setListener(CANMessageListener* listener) { _listener = listener; }

  void sendCommand(const char* command, bool expectOK = true);

  void sendCommand(const String& command, bool expectOK = true);

  void startMonitoring();

  void stopMonitoring();

  void update();

 protected:
  String rx();

  void tx(const String& command);

 private:
  BLESerialConnection& _connection;
  const CANMessageDefinitionCollection& _messageDefinitions;
  BLESerialStream* _stream;
  CANMessageListener* _listener;
  bool _monitoring = false;
};
