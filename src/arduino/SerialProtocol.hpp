#pragma once

#include <Arduino.h>
#include "shared/LocINOTypes.hpp"
#include "shared/ProtocolCodec.hpp"

namespace LocINO {

class SerialProtocol {
public:
    explicit SerialProtocol(HardwareSerial& serial);

    void begin(unsigned long baudRate);

    bool receiveCommand(CommandPacket& command);

    void sendEvent(CpuEventType type, const uint8_t* payload, uint8_t length);
    void sendEvent(CpuEventType type, const LoRaPacket& packet);

    void sendLoRaReceived(const LoRaPacket& packet);
    void sendLoRaTxOk();
    void sendLoRaTxFail();
    void sendLoRaTimeout();
    void sendAck();
    void sendError();

private:
    HardwareSerial& _serial;
};

}