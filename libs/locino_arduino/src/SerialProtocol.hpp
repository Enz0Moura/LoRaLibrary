#pragma once

#include <Arduino.h>
#include <LocINO/Types.hpp>
#include <LocINO/ProtocolCodec.hpp>

namespace LocINO {

/**
 * @brief Arduino-side serial protocol adapter for host CPU communication.
 *
 * SerialProtocol encodes controller events and decodes host commands using the
 * shared LocINO serial frame format. It bridges a hardware serial interface
 * with the LoRa protocol layer running on the Arduino.
 */
class SerialProtocol {
public:
    /**
     * @brief Creates a serial protocol adapter around an Arduino serial port.
     *
     * @param serial HardwareSerial instance used for host communication.
     */
    explicit SerialProtocol(HardwareSerial& serial);

    /**
     * @brief Starts the underlying serial interface.
     *
     * @param baudRate Baud rate used for host-controller communication.
     */
    void begin(unsigned long baudRate);

    /**
     * @brief Attempts to receive and decode one command from the host CPU.
     *
     * @param command Receives the decoded command type and payload on success.
     * @return true if a complete, valid command frame was decoded.
     * @return false if no complete command is available or validation fails.
     */
    bool receiveCommand(CommandPacket& command);

    /**
     * @brief Sends an event frame with a raw payload to the host CPU.
     *
     * @param type Event type to encode in the frame.
     * @param payload Pointer to event payload bytes. May be nullptr when length is zero.
     * @param length Number of payload bytes to send.
     */
    void sendEvent(CpuEventType type, const uint8_t* payload, uint8_t length);

    /**
     * @brief Sends an event frame whose payload is a LoRa packet.
     *
     * @param type Event type to encode in the frame.
     * @param packet LoRa packet used as the event payload.
     */
    void sendEvent(CpuEventType type, const LoRaPacket& packet);

    /**
     * @brief Reports a successfully received LoRa packet to the host CPU.
     *
     * @param packet Received LoRa packet to include in the event payload.
     */
    void sendLoRaReceived(const LoRaPacket& packet);
    /** @brief Reports successful LoRa transmission to the host CPU. */
    void sendLoRaTxOk();
    /** @brief Reports failed LoRa transmission to the host CPU. */
    void sendLoRaTxFail();
    /** @brief Reports a LoRa receive or acknowledgement timeout to the host CPU. */
    void sendLoRaTimeout();
    /** @brief Sends a generic acknowledgement event to the host CPU. */
    void sendAck();
    /** @brief Sends a generic error event to the host CPU. */
    void sendError();

private:
    HardwareSerial& _serial;
};

}