#pragma once

#include "Types.hpp"

namespace LocINO {

inline uint8_t calculateChecksum(
    uint8_t type,
    const uint8_t* payload,
    uint8_t length
) {
    uint8_t checksum = type ^ length;

    for (uint8_t i = 0; i < length; i++) {
        checksum ^= payload[i];
    }

    return checksum;
}

inline uint8_t encodeFrame(
    uint8_t type,
    const uint8_t* payload,
    uint8_t length,
    uint8_t* output
) {
    output[0] = SERIAL_START;
    output[1] = type;
    output[2] = length;

    if (payload != nullptr && length > 0) {
        memcpy(output + 3, payload, length);
    }

    output[3 + length] = calculateChecksum(type, payload, length);

    return 4 + length;
}

inline bool decodeFrame(
    const uint8_t* frame,
    uint8_t frameLength,
    uint8_t& type,
    uint8_t* payload,
    uint8_t& payloadLength
) {
    if (frameLength < 4) {
        return false;
    }

    if (frame[0] != SERIAL_START) {
        return false;
    }

    type = frame[1];
    payloadLength = frame[2];

    if (frameLength != payloadLength + 4) {
        return false;
    }

    if (payloadLength > 0) {
        memcpy(payload, frame + 3, payloadLength);
    }

    uint8_t receivedChecksum = frame[3 + payloadLength];
    uint8_t expectedChecksum = calculateChecksum(type, payload, payloadLength);

    return receivedChecksum == expectedChecksum;
}

}