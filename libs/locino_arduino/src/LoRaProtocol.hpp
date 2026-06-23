#pragma once

#include <Arduino.h>
#include "LoRaRadio.hpp"

#include <LocINO/Types.hpp>

namespace LocINO {

/**
 * @brief Arduino-side LoRa radio protocol wrapper for LocINO packets.
 *
 * LoRaProtocol owns the high-level transmission and reception workflow around
 * a LoRaRadio driver instance. It applies the LocINO packet
 * header rules, handles radio initialization/reset, and provides helpers for
 * request/acknowledgement style communication.
 */
class LoRaProtocol {
public:
    /**
     * @brief Creates a LoRa protocol controller around an RH_RF95 radio driver.
     *
     * @param radio LoRaRadio driver instance used for all LoRa operations.
     * @param frequency Radio frequency, in MHz, used during initialization.
     */
    LoRaProtocol(LoRaRadio& radio, uint8_t resetPin, float frequency);

    /**
     * @brief Initializes and configures the LoRa radio module.
     *
     * @return true if the radio reset, driver initialization, and frequency
     * configuration complete successfully.
     * @return false if the radio driver cannot be initialized or configured.
     */
    bool begin();

    /**
     * @brief Sends a raw LocINO LoRa payload.
     *
     * @param data Pointer to the bytes that should be transmitted.
     * @param length Number of bytes to transmit from @p data.
     * @return true if the radio driver reports a successful transmission.
     * @return false if transmission fails.
     */
    bool send(const uint8_t* data, uint8_t length);

    /**
     * @brief Sends a LocINO LoRa packet.
     *
     * @param packet Packet containing the data buffer and valid byte count.
     * @return true if the packet is transmitted successfully.
     * @return false if the radio driver reports a transmission failure.
     */
    bool send(const LoRaPacket& packet);

    /**
     * @brief Receives a LocINO LoRa packet with optional filtering.
     *
     * @param packet Receives the decoded packet bytes when a valid packet arrives.
     * @param timeoutMs Maximum time, in milliseconds, to wait for a packet.
     * @param mode Receive filter used to accept any packet or only acknowledgements.
     * @return LoRaStatus::Ok when a valid packet is received.
     * @return LoRaStatus::Timeout when no acceptable packet arrives before timeout.
     * @return LoRaStatus::InvalidHeader when a packet lacks the expected LocINO header.
     * @return LoRaStatus::Error when the radio driver reports a receive error.
     */
    LoRaStatus receive(
        LoRaPacket& packet,
        unsigned long timeoutMs = DEFAULT_TIMEOUT_MS,
        ReceiveMode mode = ReceiveMode::Any
    );

    /**
     * @brief Sends a beacon packet and waits for an acknowledgement.
     *
     * @param packet Beacon packet to transmit.
     * @param timeoutMs Maximum time, in milliseconds, to wait for acknowledgement.
     * @return LoRaStatus::Ok when the beacon is sent and acknowledged.
     * @return LoRaStatus::Timeout when no acknowledgement is received in time.
     * @return LoRaStatus::Error when transmission or receive handling fails.
     */
    LoRaStatus sendBeacon(
        const LoRaPacket& packet,
        unsigned long timeoutMs = DEFAULT_TIMEOUT_MS
    );

private:
    LoRaRadio& _radio;
    uint8_t _resetPin;
    float _frequency;

    /**
     * @brief Performs the hardware reset sequence for the LoRa radio module.
     */

    /**
     * @brief Finds the LocINO header position inside a received byte buffer.
     *
     * @param data Buffer to inspect.
     * @param length Number of bytes available in @p data.
     * @return Zero-based header index, or -1 if the header is not present.
     */
    int findHeader(const uint8_t* data, uint8_t length) const;

    /**
     * @brief Checks whether a buffer contains the fixed LocINO LoRa header.
     *
     * @param data Buffer to inspect.
     * @param length Number of bytes available in @p data.
     * @return true if the LocINO header is present.
     * @return false if the header cannot be found.
     */
    bool hasHeader(const uint8_t* data, uint8_t length) const;
};

}