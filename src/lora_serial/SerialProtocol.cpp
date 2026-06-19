#include "SerialProtocol.hpp"

namespace LocINO {

SerialProtocol::SerialProtocol(HardwareSerial& serial)
    : _serial(serial) {}

void SerialProtocol::begin(unsigned long baudRate) {
    _serial.begin(baudRate);
}

void SerialProtocol::send(
    SerialMessageType type,
    const uint8_t* payload,
    uint8_t length
) {
    _serial.write(SERIAL_START);
    _serial.write(static_cast<uint8_t>(type));
    _serial.write(length);

    if (payload != nullptr && length > 0) {
        _serial.write(payload, length);
    }

    _serial.write(calculateChecksum(type, payload, length));
}

void SerialProtocol::send(SerialMessageType type, const Packet& packet) {
    send(type, packet.data, packet.length);
}

void SerialProtocol::sendLoRaReceived(const Packet& packet) {
    send(SerialMessageType::LoRaRx, packet);
}

void SerialProtocol::sendLoRaTxOk() {
    send(SerialMessageType::LoRaTxOk, nullptr, 0);
}

void SerialProtocol::sendLoRaTxFail() {
    send(SerialMessageType::LoRaTxFail, nullptr, 0);
}

void SerialProtocol::sendLoRaTimeout() {
    send(SerialMessageType::LoRaTimeout, nullptr, 0);
}

void SerialProtocol::sendAck() {
    send(SerialMessageType::Ack, nullptr, 0);
}

void SerialProtocol::sendError() {
    send(SerialMessageType::Error, nullptr, 0);
}

uint8_t SerialProtocol::calculateChecksum(
    SerialMessageType type,
    const uint8_t* payload,
    uint8_t length
) const {
    uint8_t checksum = static_cast<uint8_t>(type) ^ length;

    if (payload != nullptr) {
        for (uint8_t i = 0; i < length; i++) {
            checksum ^= payload[i];
        }
    }

    return checksum;
}

}