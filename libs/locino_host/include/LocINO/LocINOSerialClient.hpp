#pragma once

#include <LocINO/Types.hpp>
#include <LocINO/ProtocolCodec.hpp>
#include "SerialPort.hpp"
#include <stdexcept>
#include <string>
#include <chrono>
#include <thread>

namespace LocINO {

class LocINOSerialClient {
public:
    LocINOSerialClient(const std::string& device, int baudRate = 9600);

    bool isReady() const;

    bool waitReady(unsigned long timeoutMs = DEFAULT_TIMEOUT_MS);

    bool sendMessage(const LoRaPacket& packet);
    bool sendBeacon(const LoRaPacket& packet);
    bool listenRecord();
    bool listenBeacon();

    bool receiveEvent(
    EventPacket& event,
    unsigned long timeout = DEFAULT_TIMEOUT_MS
    );

private:
    SerialPort _serial;

    bool _ready = false;

    bool sendCommand(CpuCommandType type, const uint8_t* payload, uint8_t length);
};

}