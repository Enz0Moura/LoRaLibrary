#pragma once

#include "Types.hpp"

namespace LocINO {

/**
 * @brief Calculates the XOR checksum used by the LocINO serial frame format.
 *
 * The checksum is computed over the frame type, payload length, and every
 * payload byte. It is intentionally lightweight for embedded serial links.
 *
 * @param type Frame type byte.
 * @param payload Pointer to the payload bytes. May be nullptr when length is zero.
 * @param length Number of payload bytes.
 * @return Computed checksum byte.
 */
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

/**
 * @brief Encodes a serial protocol frame into an output byte buffer.
 *
 * The encoded frame layout is: start byte, type, payload length, payload,
 * checksum. The caller must ensure that @p output has room for length + 4 bytes.
 *
 * @param type Frame type byte.
 * @param payload Pointer to payload bytes. May be nullptr when length is zero.
 * @param length Number of payload bytes to encode.
 * @param output Destination buffer that receives the encoded frame.
 * @return Total number of bytes written to @p output.
 */
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

/**
 * @brief Decodes and validates a LocINO serial protocol frame.
 *
 * The function validates the start byte, expected frame length, and checksum
 * before reporting success. On success, the decoded type and payload are copied
 * to the caller-provided output parameters.
 *
 * @param frame Pointer to the encoded frame bytes.
 * @param frameLength Number of bytes available in @p frame.
 * @param type Receives the decoded frame type.
 * @param payload Destination buffer for decoded payload bytes.
 * @param payloadLength Receives the decoded payload length.
 * @return true if the frame is structurally valid and checksum verification passes.
 * @return false if the frame is malformed or checksum verification fails.
 */
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