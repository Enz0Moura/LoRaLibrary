#include "LoRaProtocol.hpp"

namespace LocINO {

LoRaProtocol::LoRaProtocol(
    RH_RF95& radio,
    uint8_t resetPin,
    float frequency
) : _radio(radio),
    _resetPin(resetPin),
    _frequency(frequency) {}

bool LoRaProtocol::begin() {
    pinMode(_resetPin, OUTPUT);
    digitalWrite(_resetPin, HIGH);

    delay(100);

    resetRadio();

    if (!_radio.init()) {
        return false;
    }

    if (!_radio.setFrequency(_frequency)) {
        return false;
    }

    _radio.setTxPower(14, false);
    _radio.setModemConfig(RH_RF95::Bw125Cr45Sf128);

    return true;
}

bool LoRaProtocol::send(const uint8_t* data, uint8_t length) {
    if (data == nullptr || length == 0) {
        return false;
    }

    int headerIndex = findHeader(data, length);

    if (headerIndex < 0) {
        return false;
    }

    uint8_t finalLength = length - headerIndex;

    _radio.send(data + headerIndex, finalLength);
    _radio.waitPacketSent();

    return true;
}

bool LoRaProtocol::send(const Packet& packet) {
    return send(packet.data, packet.length);
}

LoRaStatus LoRaProtocol::receive(
    Packet& packet,
    unsigned long timeoutMs,
    ReceiveMode mode
) {
    unsigned long startTime = millis();

    while (millis() - startTime < timeoutMs) {
        if (!_radio.available()) {
            continue;
        }

        uint8_t len = MAX_PACKET_SIZE;

        if (!_radio.recv(packet.data, &len)) {
            packet.length = 0;
            return LoRaStatus::Error;
        }

        packet.length = len;

        if (!hasHeader(packet.data, packet.length)) {
            return LoRaStatus::InvalidHeader;
        }

        if (mode == ReceiveMode::AckOnly && !isAckPacket(packet)) {
            return LoRaStatus::InvalidHeader;
        }

        return LoRaStatus::Ok;
    }

    packet.length = 0;
    return LoRaStatus::Timeout;
}

LoRaStatus LoRaProtocol::sendBeacon(
    const uint8_t* data,
    uint8_t length,
    unsigned long timeoutMs
) {
    if (!send(data, length)) {
        return LoRaStatus::Error;
    }

    Packet response;

    return receive(
        response,
        timeoutMs,
        ReceiveMode::AckOnly
    );
}

LoRaStatus LoRaProtocol::sendBeacon(
    const Packet& packet,
    unsigned long timeoutMs
) {
    return sendBeacon(packet.data, packet.length, timeoutMs);
}

void LoRaProtocol::resetRadio() {
    digitalWrite(_resetPin, LOW);
    delay(10);
    digitalWrite(_resetPin, HIGH);
    delay(10);
}

int LoRaProtocol::findHeader(const uint8_t* data, uint8_t length) const {
    if (data == nullptr || length < HEADER_SIZE) {
        return -1;
    }

    for (uint8_t i = 0; i < length - 1; i++) {
        if (data[i] == LORA_HEADER_1 && data[i + 1] == LORA_HEADER_2) {
            return i;
        }
    }

    return -1;
}

bool LoRaProtocol::hasHeader(const uint8_t* data, uint8_t length) const {
    return data != nullptr &&
           length >= HEADER_SIZE &&
           data[0] == LORA_HEADER_1 &&
           data[1] == LORA_HEADER_2;
}

}