#include "LocINOSerialClient.hpp"

namespace LocINO {

LocINOSerialClient::LocINOSerialClient(
    const std::string& device,
    int baudRate
) : _serial(device, baudRate) {
    if (!waitReady()) {
        throw std::runtime_error(
            "Arduino did not send ready ACK"
        );
    }
}

bool LocINOSerialClient::waitReady(unsigned long timeoutMs) {
    EventPacket event;

    while (receiveEvent(event)) {
        if (event.type == CpuEventType::Ack) {
            return true;
        }

        if (event.type == CpuEventType::Error) {
            return false;
        }
    }

    return false;
}

bool LocINOSerialClient::sendMessage(const LoRaPacket& packet) {
    return sendCommand(CpuCommandType::SendMessage, packet.data, packet.length);
}

bool LocINOSerialClient::sendBeacon(const LoRaPacket& packet) {
    return sendCommand(CpuCommandType::SendBeacon, packet.data, packet.length);
}

bool LocINOSerialClient::listenRecord() {
    return sendCommand(CpuCommandType::ListenRecord, nullptr, 0);
}

bool LocINOSerialClient::listenBeacon() {
    return sendCommand(CpuCommandType::ListenBeacon, nullptr, 0);
}

bool LocINOSerialClient::sendCommand(
    CpuCommandType type,
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

    return _serial.writeBytes(frame, frameLength);
}

bool LocINOSerialClient::receiveEvent(EventPacket& event) {
    uint8_t start = 0;

    do {
        if (!_serial.readByte(start)) {
            return false;
        }
    } while (start != SERIAL_START);

    uint8_t type = 0;
    uint8_t length = 0;

    if (!_serial.readByte(type)) {
        return false;
    }

    if (!_serial.readByte(length)) {
        return false;
    }

    if (length > MAX_PACKET_SIZE) {
        return false;
    }

    event.type = static_cast<CpuEventType>(type);
    event.length = length;

    if (length > 0 && !_serial.readBytes(event.data, length)) {
        return false;
    }

    uint8_t receivedChecksum = 0;

    if (!_serial.readByte(receivedChecksum)) {
        return false;
    }

    uint8_t expectedChecksum = calculateChecksum(
        type,
        event.data,
        event.length
    );

    return receivedChecksum == expectedChecksum;
}

}