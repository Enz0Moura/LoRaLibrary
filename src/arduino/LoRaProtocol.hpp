#pragma once

#include <Arduino.h>
#include <RH_RF95.h>

#include "shared/LocINOTypes.hpp"

namespace LocINO {

class LoRaProtocol {
public:
    LoRaProtocol(RH_RF95& radio, uint8_t resetPin, float frequency);

    bool begin();

    bool send(const uint8_t* data, uint8_t length);
    bool send(const LoRaPacket& packet);

    LoRaStatus receive(
        LoRaPacket& packet,
        unsigned long timeoutMs = DEFAULT_TIMEOUT_MS,
        ReceiveMode mode = ReceiveMode::Any
    );

    LoRaStatus sendBeacon(
        const LoRaPacket& packet,
        unsigned long timeoutMs = DEFAULT_TIMEOUT_MS
    );

private:
    RH_RF95& _radio;
    uint8_t _resetPin;
    float _frequency;

    void resetRadio();

    int findHeader(const uint8_t* data, uint8_t length) const;
    bool hasHeader(const uint8_t* data, uint8_t length) const;
};

}