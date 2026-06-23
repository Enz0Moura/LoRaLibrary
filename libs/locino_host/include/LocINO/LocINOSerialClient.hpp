#pragma once

#include <LocINO/Types.hpp>
#include <LocINO/ProtocolCodec.hpp>
#include "SerialPort.hpp"
#include <stdexcept>
#include <string>
#include <chrono>
#include <thread>

namespace LocINO {

/**
 * @brief Host-side client for communicating with a LocINO Arduino controller.
 *
 * LocINOSerialClient wraps the serial frame protocol used between a host
 * computer and the Arduino-side LoRa firmware. It provides convenience methods
 * for sending LoRa commands and receiving controller events.
 */
class LocINOSerialClient {
public:
    /**
     * @brief Opens and configures a serial connection to the LocINO controller.
     *
     * @param device Platform-specific serial device path or port name.
     * @param baudRate Serial baud rate used by the controller firmware.
     */
    LocINOSerialClient(const std::string& device, int baudRate = 9600);

    /**
     * @brief Reports whether the controller has completed the ready handshake.
     *
     * @return true if the controller is ready to accept commands.
     * @return false if the ready handshake has not completed yet.
     */
    bool isReady() const;

    /**
     * @brief Waits for the controller to report readiness.
     *
     * @param timeoutMs Maximum time, in milliseconds, to wait for readiness.
     * @return true if the controller becomes ready before the timeout.
     * @return false if readiness is not observed before the timeout expires.
     */
    bool waitReady(unsigned long timeoutMs = DEFAULT_TIMEOUT_MS);

    /**
     * @brief Requests transmission of a regular LoRa message packet.
     *
     * @param packet Packet to transmit through the Arduino LoRa controller.
     * @return true if the command frame was sent successfully.
     * @return false if serial transmission fails.
     */
    bool sendMessage(const LoRaPacket& packet);

    /**
     * @brief Requests transmission of a LoRa beacon packet.
     *
     * @param packet Beacon packet to transmit through the Arduino LoRa controller.
     * @return true if the command frame was sent successfully.
     * @return false if serial transmission fails.
     */
    bool sendBeacon(const LoRaPacket& packet);

    /**
     * @brief Requests that the controller listen for a regular LoRa record.
     *
     * @return true if the listen command was sent successfully.
     * @return false if serial transmission fails.
     */
    bool listenRecord();

    /**
     * @brief Requests that the controller listen for a LoRa beacon packet.
     *
     * @return true if the listen command was sent successfully.
     * @return false if serial transmission fails.
     */
    bool listenBeacon();

    /**
     * @brief Receives and decodes one event from the controller.
     *
     * @param event Receives the decoded event type and payload on success.
     * @param timeout Maximum time, in milliseconds, to wait for a complete event.
     * @return true if a valid event frame is received before the timeout.
     * @return false if the timeout expires or frame validation fails.
     */
    bool receiveEvent(
    EventPacket& event,
    unsigned long timeout = DEFAULT_TIMEOUT_MS
    );

private:
    SerialPort _serial;

    bool _ready = false;

    /**
     * @brief Encodes and writes a command frame to the controller.
     *
     * @param type Command type to send.
     * @param payload Pointer to command payload bytes. May be nullptr when length is zero.
     * @param length Number of payload bytes to encode.
     * @return true if the complete command frame was written successfully.
     * @return false if the serial write operation fails.
     */
    bool sendCommand(CpuCommandType type, const uint8_t* payload, uint8_t length);
};

}