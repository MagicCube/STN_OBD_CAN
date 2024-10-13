# STN OBD CAN Library for Arduino

A simple but practical CAN Bus communication library for the STN family.

## Features

- Supports HS-CAN (High Speed CAN), ISO15765-4 (11-bit ID, 500 kbaud)
- Real-time CAN bus communication over BLE
- CAN message decoding and processing
- Memory leak-free

## Requirements

- ESP32 family
    - Tested on classic ESP32 and ESP32-S3
- A BLE OBD device that supports both AT and ST commands
    - Tested on VGate VLinker MC+
    - Should work on OBDLink STN1xxx family
    - Other inexpensive OBD devices are not supported
- Tested on Tesla Model 3 only
