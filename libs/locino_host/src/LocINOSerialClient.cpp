#include "LocINO/LocINOSerialClient.hpp"

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
    auto start = std::chrono::steady_clock::now();

    while (true) {
        EventPacket event;

        if (receiveEvent(event)) {
            if (event.type == CpuEventType::Ack) {
                return true;
            }

            if (event.type == CpuEventType::Error) {
                return false;
            }
        }

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - start
        ).count();

        if (elapsed >= timeoutMs) {
            return false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

bool LocINOSerialClient::sendMessage(const LoRaPacket& packet) {
    return sendCommand(CpuCommandType::LoRaSendMessage, packet.data, packet.length);
}

bool LocINOSerialClient::sendBeacon(const LoRaPacket& packet) {
    return sendCommand(CpuCommandType::LoRaSendBeacon, packet.data, packet.length);
}

bool LocINOSerialClient::listenRecord() {
    return sendCommand(CpuCommandType::LoRaListenRecord, nullptr, 0);
}

bool LocINOSerialClient::listenBeacon() {
    return sendCommand(CpuCommandType::LoRaListenBeacon, nullptr, 0);
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

bool LocINOSerialClient::receiveEvent(
    EventPacket& event,
    unsigned long timeoutMs
) {
    const auto deadline =
        std::chrono::steady_clock::now() +
        std::chrono::milliseconds(timeoutMs);

    auto remainingMs = [&]() -> unsigned long {
        auto now = std::chrono::steady_clock::now();

        if (now >= deadline) {
            return 0;
        }

        return static_cast<unsigned long>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                deadline - now
            ).count()
        );
    };

    uint8_t start = 0;

    do {
        if (!_serial.readByte(start, remainingMs())) {
            return false;
        }
    } while (start != SERIAL_START);

    uint8_t type = 0;
    uint8_t length = 0;

    if (!_serial.readByte(type, remainingMs())) {
        return false;
    }

    if (!_serial.readByte(length, remainingMs())) {
        return false;
    }

    if (length > MAX_PACKET_SIZE) {
        return false;
    }

    event.type = static_cast<CpuEventType>(type);
    event.length = length;

    if (length > 0 && !_serial.readBytes(event.data, length, remainingMs())) {
        return false;
    }

    uint8_t receivedChecksum = 0;

    if (!_serial.readByte(receivedChecksum, remainingMs())) {
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