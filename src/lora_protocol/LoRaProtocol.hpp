#pragma once

#include <Arduino.h>
#include <RH_RF95.h>
#include "../lora_types/LoCINOTypes.hpp"
#include "../lora_serial/SerialProtocol.hpp"

namespace LocINO {

class LoRaProtocol {
public:
    LoRaProtocol(
        RH_RF95& radio,
        uint8_t resetPin,
        float frequency
    );

    bool begin();

    bool send(const uint8_t* data, uint8_t length);
    bool send(const Packet& packet);

    LoRaStatus receive(
        Packet& packet,
        unsigned long timeoutMs = 10000,
        ReceiveMode mode = ReceiveMode::Any
    );

    LoRaStatus sendBeacon(
        const uint8_t* data,
        uint8_t length,
        unsigned long timeoutMs = 10000
    );

    LoRaStatus sendBeacon(
        const Packet& packet,
        unsigned long timeoutMs = 10000
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