#pragma once

#include <Arduino.h>
#include "../lora_types/LoCINOTypes.hpp"

namespace LocINO {

class SerialProtocol {
public:
    explicit SerialProtocol(HardwareSerial& serial);

    void begin(unsigned long baudRate);

    void send(SerialMessageType type, const uint8_t* payload, uint8_t length);
    void send(SerialMessageType type, const Packet& packet);

    void sendLoRaReceived(const Packet& packet);
    void sendLoRaTxOk();
    void sendLoRaTxFail();
    void sendLoRaTimeout();
    void sendAck();
    void sendError();

private:
    HardwareSerial& _serial;

    uint8_t calculateChecksum(
        SerialMessageType type,
        const uint8_t* payload,
        uint8_t length
    ) const;
};

}