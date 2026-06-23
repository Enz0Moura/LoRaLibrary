#pragma once

#include <stdint.h>
#include <string.h>

namespace LocINO {

/** Start byte used to identify the beginning of every serial protocol frame. */
constexpr uint8_t SERIAL_START = 0x7E;

/** First byte of the fixed LoRa packet header. */
constexpr uint8_t LORA_HEADER_1 = 0xFF;
/** Second byte of the fixed LoRa packet header. */
constexpr uint8_t LORA_HEADER_2 = 0xFF;

/** Standard application message payload length, including the LoRa header. */
constexpr uint8_t MESSAGE_LENGTH = 21;
/** Standard beacon payload length, including the LoRa header. */
constexpr uint8_t BEACON_LENGTH = 10;
/** Length of an acknowledgement LoRa packet. */
constexpr uint8_t ACK_LENGTH = 2;
/** Number of bytes reserved for the fixed LoRa packet header. */
constexpr uint8_t HEADER_SIZE = 2;

/** Maximum payload size accepted by the shared LocINO packet structures. */
constexpr uint8_t MAX_PACKET_SIZE = 251;
/** Default timeout, in milliseconds, used by blocking protocol operations. */
constexpr unsigned long DEFAULT_TIMEOUT_MS = 10000;

/**
 * @brief Commands sent from the host CPU to the Arduino/LoRa controller.
 */
enum class CpuCommandType : uint8_t {
    /** Request transmission of a regular LoRa message packet. */
    LoRaSendMessage = 0x10,
    /** Request transmission of a LoRa beacon packet. */
    LoRaSendBeacon = 0x11,
    /** Request listening for a regular LoRa record/message. */
    LoRaListenRecord = 0x12,
    /** Request listening for a LoRa beacon. */
    LoRaListenBeacon = 0x13
};

/**
 * @brief Events emitted by the Arduino/LoRa controller to the host CPU.
 */
enum class CpuEventType : uint8_t {
    /** A LoRa packet was received successfully. */
    LoRaRx = 0x01,
    /** A LoRa transmission completed successfully. */
    LoRaTxOk = 0x02,
    /** A LoRa transmission failed. */
    LoRaTxFail = 0x03,
    /** A LoRa receive or acknowledgement wait operation timed out. */
    LoRaTimeout = 0x04,
    /** Command or packet acknowledgement event. */
    Ack = 0x05,
    /** Generic protocol or radio error event. */
    Error = 0x06,
    /** Diagnostic event intended for debugging output. */
    Debug = 0x07
};

/**
 * @brief Result codes returned by LoRa radio operations.
 */
enum class LoRaStatus : int8_t {
    /** Operation completed successfully. */
    Ok = 1,
    /** Operation timed out before a valid packet was received. */
    Timeout = 0,
    /** Radio driver or protocol operation failed. */
    Error = -1,
    /** Received packet did not contain the expected LocINO header. */
    InvalidHeader = -2
};

/**
 * @brief Packet filtering mode used by receive operations.
 */
enum class ReceiveMode : uint8_t {
    /** Accept any valid LocINO LoRa packet. */
    Any,
    /** Accept only acknowledgement packets. */
    AckOnly
};

/**
 * @brief Fixed-size container for a LocINO LoRa packet.
 *
 * The packet stores raw bytes exactly as they are transmitted over LoRa.
 * The first two bytes are normally the fixed LocINO LoRa header.
 */
struct LoRaPacket {
    /** Raw packet bytes. Only the first @ref length bytes are valid. */
    uint8_t data[MAX_PACKET_SIZE];
    /** Number of valid bytes stored in @ref data. */
    uint8_t length = 0;

    /**
     * @brief Writes the fixed LocINO LoRa header into the first two bytes.
     */
    void addHeader(){
        data[0] = LORA_HEADER_1;
        data[1] = LORA_HEADER_2;
    };

    /**
     * @brief Creates a packet with the specified length and default header.
     *
     * @param length Number of bytes that will be considered valid.
     * @return LoRaPacket initialized with the LocINO header.
     */
    static LoRaPacket createWithHeader(uint8_t length){
        LoRaPacket packet;
        packet.length = length;
        packet.addHeader();
        return packet;
    }

    /**
     * @brief Creates a LoRa packet from a null-terminated text string.
     *
     * The text is copied after the fixed two-byte LocINO header.
     *
     * @param text Null-terminated text payload to copy into the packet.
     * @return LoRaPacket containing the LocINO header followed by the text bytes.
     */
    static LoRaPacket fromText(const char* text) {
        LoRaPacket packet;

        packet.addHeader();

        uint8_t textLength =
            static_cast<uint8_t>(strlen(text));

        packet.length = HEADER_SIZE + textLength;

        memcpy(
            packet.data + HEADER_SIZE,
            text,
            textLength
        );

        return packet;
    }

};

/**
 * @brief Command frame payload decoded from the host serial protocol.
 */
struct CommandPacket {
    /** Command identifier describing the requested operation. */
    CpuCommandType type;
    /** Command payload bytes. Only the first @ref length bytes are valid. */
    uint8_t data[MAX_PACKET_SIZE];
    /** Number of valid bytes stored in @ref data. */
    uint8_t length = 0;

    /**
     * @brief Converts this command payload into a LoRa packet container.
     *
     * @return LoRaPacket containing a copy of the command payload bytes.
     */
    LoRaPacket toLoRaPacket() const {
        LoRaPacket packet;
        packet.length = length;
        memcpy(packet.data, data, length);
        return packet;
    }
};

/**
 * @brief Event frame payload emitted by the Arduino/LoRa controller.
 */
struct EventPacket {
    /** Event identifier describing the reported operation result. */
    CpuEventType type;
    /** Event payload bytes. Only the first @ref length bytes are valid. */
    uint8_t data[MAX_PACKET_SIZE];
    /** Number of valid bytes stored in @ref data. */
    uint8_t length = 0;
};

/**
 * @brief Creates the standard LocINO LoRa acknowledgement packet.
 *
 * @return LoRaPacket containing only the fixed two-byte LocINO header.
 */
inline LoRaPacket createAck() {
    LoRaPacket packet;
    packet.data[0] = LORA_HEADER_1;
    packet.data[1] = LORA_HEADER_2;
    packet.length = ACK_LENGTH;
    return packet;
}

/**
 * @brief Creates a LocINO LoRa packet with the default header.
 *
 * @param length Number of valid bytes to expose in the packet.
 * @return LoRaPacket initialized with the fixed LocINO header.
 */
inline LoRaPacket createPacket(uint8_t length) {
    LoRaPacket packet;

    packet.length = length;
    packet.addHeader();
    
    return packet;
}

/**
 * @brief Checks whether a packet matches the standard acknowledgement packet.
 *
 * @param packet Packet to compare against the LocINO acknowledgement format.
 * @return true if the packet is an acknowledgement packet.
 * @return false if the packet length or contents differ from the acknowledgement format.
 */
inline bool isAckPacket(const LoRaPacket& packet) {
    LoRaPacket ack = createAck();

    return packet.length == ack.length &&
           memcmp(packet.data, ack.data, ack.length) == 0;
}

}