#pragma once

#include <stdint.h>
#include <string.h>

namespace LocINO {

constexpr uint8_t SERIAL_START = 0x7E;

constexpr uint8_t LORA_HEADER_1 = 0xFF;
constexpr uint8_t LORA_HEADER_2 = 0xFF;

constexpr uint8_t MESSAGE_LENGTH = 21;
constexpr uint8_t BEACON_LENGTH = 10;
constexpr uint8_t ACK_LENGTH = 2;
constexpr uint8_t HEADER_SIZE = 2;

constexpr uint8_t MAX_PACKET_SIZE = 251;
constexpr unsigned long DEFAULT_TIMEOUT_MS = 10000;

enum class CpuCommandType : uint8_t {
    SendMessage = 0x10,
    SendBeacon = 0x11,
    ListenRecord = 0x12,
    ListenBeacon = 0x13
};

enum class CpuEventType : uint8_t {
    LoRaRx = 0x01,
    LoRaTxOk = 0x02,
    LoRaTxFail = 0x03,
    LoRaTimeout = 0x04,
    Ack = 0x05,
    Error = 0x06,
    Debug = 0x07
};

enum class LoRaStatus : int8_t {
    Ok = 1,
    Timeout = 0,
    Error = -1,
    InvalidHeader = -2
};

enum class ReceiveMode : uint8_t {
    Any,
    AckOnly
};

struct LoRaPacket {
    uint8_t data[MAX_PACKET_SIZE];
    uint8_t length = 0;

    void addHeader(){
        data[0] = LORA_HEADER_1;
        data[1] = LORA_HEADER_2;
    };

    static LoRaPacket createWithHeader(uint8_t length){
        LoRaPacket packet;
        packet.length = length;
        packet.addHeader();
        return packet;
    }
};

struct CommandPacket {
    CpuCommandType type;
    uint8_t data[MAX_PACKET_SIZE];
    uint8_t length = 0;

    LoRaPacket toLoRaPacket() const {
        LoRaPacket packet;
        packet.length = length;
        memcpy(packet.data, data, length);
        return packet;
    }
};

struct EventPacket {
    CpuEventType type;
    uint8_t data[MAX_PACKET_SIZE];
    uint8_t length = 0;
};

inline LoRaPacket createAck() {
    LoRaPacket packet;
    packet.data[0] = LORA_HEADER_1;
    packet.data[1] = LORA_HEADER_2;
    packet.length = ACK_LENGTH;
    return packet;
}

inline LoRaPacket createPacket(uint8_t length) {
    LoRaPacket packet;

    packet.length = length;
    packet.addHeader();
    
    return packet;
}

inline bool isAckPacket(const LoRaPacket& packet) {
    LoRaPacket ack = createAck();

    return packet.length == ack.length &&
           memcmp(packet.data, ack.data, ack.length) == 0;
}

}