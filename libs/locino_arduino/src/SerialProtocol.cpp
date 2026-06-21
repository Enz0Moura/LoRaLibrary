#include "SerialProtocol.hpp"

namespace LocINO {

SerialProtocol::SerialProtocol(HardwareSerial& serial)
    : _serial(serial) {}

void SerialProtocol::begin(unsigned long baudRate) {
    _serial.begin(baudRate);
}

void SerialProtocol::sendEvent(
    CpuEventType type,
    const uint8_t* payload,
    uint8_t length
) {
    uint8_t frame[MAX_PACKET_SIZE + 4];

    uint8_t frameLength = encodeFrame(
        static_cast<uint8_t>(type),
        payload,
        length,
        frame
    );

    _serial.write(frame, frameLength);
}

void SerialProtocol::sendEvent(CpuEventType type, const LoRaPacket& packet) {
    sendEvent(type, packet.data, packet.length);
}

bool SerialProtocol::receiveCommand(CommandPacket& command) {
    if (_serial.available() < 4) {
        return false;
    }

    while (_serial.available() > 0 && _serial.peek() != SERIAL_START) {
        _serial.read();
    }

    if (_serial.available() < 4) {
        return false;
    }

    uint8_t start = _serial.read();
    uint8_t type = _serial.read();
    uint8_t length = _serial.read();

    if (start != SERIAL_START || length > MAX_PACKET_SIZE) {
        return false;
    }

    unsigned long startTime = millis();

    while (_serial.available() < length + 1) {
        if (millis() - startTime > DEFAULT_TIMEOUT_MS) {
            return false;
        }
    }

    command.type = static_cast<CpuCommandType>(type);
    command.length = length;

    if (length > 0) {
        _serial.readBytes(command.data, length);
    }

    uint8_t receivedChecksum = _serial.read();
    uint8_t expectedChecksum = calculateChecksum(type, command.data, length);

    return receivedChecksum == expectedChecksum;
}

void SerialProtocol::sendLoRaReceived(const LoRaPacket& packet) {
    sendEvent(CpuEventType::LoRaRx, packet);
}

void SerialProtocol::sendLoRaTxOk() {
    sendEvent(CpuEventType::LoRaTxOk, nullptr, 0);
}

void SerialProtocol::sendLoRaTxFail() {
    sendEvent(CpuEventType::LoRaTxFail, nullptr, 0);
}

void SerialProtocol::sendLoRaTimeout() {
    sendEvent(CpuEventType::LoRaTimeout, nullptr, 0);
}

void SerialProtocol::sendAck() {
    sendEvent(CpuEventType::Ack, nullptr, 0);
}

void SerialProtocol::sendError() {
    sendEvent(CpuEventType::Error, nullptr, 0);
}

}