#pragma once

#include <Arduino.h>
#include <RH_RF95.h>

namespace LocINO {

constexpr uint8_t MESSAGE_LENGTH = 21;
constexpr uint8_t BEACON_LENGTH = 10;
constexpr uint8_t ACK_LENGTH = 2;

constexpr uint8_t HEADER_SIZE = 2;


constexpr uint8_t LORA_HEADER_1 = 0xFF;
constexpr uint8_t LORA_HEADER_2 = 0xFF;

constexpr uint8_t SERIAL_START = 0x7E;
constexpr uint8_t MAX_PACKET_SIZE = RH_RF95_MAX_MESSAGE_LEN;

enum class LoRaStatus : int8_t {
    Ok = 1,
    Timeout = 0,
    Error = -1,
    InvalidHeader = -2
};

enum class SerialMessageType : uint8_t {
    LoRaRx = 0x01,
    LoRaTxOk = 0x02,
    LoRaTxFail = 0x03,
    LoRaTimeout = 0x04,
    Ack = 0x05,
    Error = 0x06,
    Debug = 0x07
};

enum class ReceiveMode : uint8_t {
    Any,
    AckOnly
};

struct Packet {
    uint8_t data[MAX_PACKET_SIZE];
    uint8_t length = 0;
};

inline Packet createAck() {
    Packet packet;

    packet.data[0] = LORA_HEADER_1;
    packet.data[1] = LORA_HEADER_2;
    packet.length = ACK_LENGTH;

    return packet;
}

inline bool isAckPacket(const Packet& packet) {
    return packet.length == ACK_LENGTH &&
           packet.data[0] == LORA_HEADER_1 &&
           packet.data[1] == LORA_HEADER_2;
}

}