#pragma once

#include <Arduino.h>
#include <SPI.h>

namespace LocINO {

/**
 * @brief Low-level SPI driver for an SX127x/RFM95-compatible LoRa radio.
 *
 * LoRaRadio provides direct radio-module control for Arduino environments.
 * It configures the LoRa transceiver, manages transmit and receive modes,
 * and exposes raw byte-oriented packet send/receive operations.
 *
 * This class is intended to be used by higher-level protocol wrappers such as
 * LoRaProtocol, but it can also be used directly when only raw LoRa payload
 * transmission and reception are required.
 *
 * @note The implementation assumes a Semtech SX127x-compatible register map
 *       and validates the radio by checking the version register for 0x12.
 * @note SPI is initialized inside init() by calling SPI.begin().
 */
class LoRaRadio {
public:
    /**
     * @brief Creates a LoRa radio driver bound to the given hardware pins.
     *
     * @param csPin SPI chip-select pin connected to the LoRa module NSS/CS pin.
     * @param irqPin Interrupt/DIO0 pin connected to the LoRa module IRQ output.
     * @param resetPin Reset pin connected to the LoRa module reset input.
     */
    LoRaRadio(uint8_t csPin, uint8_t irqPin, uint8_t resetPin);

    /**
     * @brief Initializes the LoRa module and applies default radio settings.
     *
     * This method configures GPIO pins, starts SPI, performs a hardware reset,
     * validates the radio version register, initializes FIFO base addresses,
     * configures LNA/modem defaults, sets the sync word, clears IRQ flags, and
     * leaves the radio in standby mode.
     *
     * @return true if the radio is detected and initialized successfully.
     * @return false if the expected radio version register is not detected.
     */
    bool init();

    /**
     * @brief Sets the RF carrier frequency.
     *
     * @param frequencyMHz Frequency in MHz, for example 915.0 or 868.0.
     * @return true after writing the calculated frequency registers.
     *
     * @note The caller is responsible for using a frequency legal for the
     *       region and supported by the connected LoRa module.
     */
    bool setFrequency(float frequencyMHz);

    /**
     * @brief Sets the transmit output power.
     *
     * @param powerDbm Requested output power in dBm. Values are clamped to the
     *        supported range of 2 dBm to 17 dBm before writing PA configuration.
     */
    void setTxPower(uint8_t powerDbm);

    /**
     * @brief Applies the default modem configuration used by this library.
     *
     * The configured LoRa parameters are:
     * - Bandwidth: 125 kHz
     * - Coding rate: 4/5
     * - Spreading factor: SF7 (128 chips/symbol)
     * - Explicit header mode
     * - CRC enabled
     * - AGC auto enabled
     */
    void setModemConfigBw125Cr45Sf128();

    /**
     * @brief Starts transmission of a raw LoRa payload.
     *
     * The payload is written into the radio FIFO, the payload length register is
     * updated, and the radio is switched to TX mode.
     *
     * @param data Pointer to the payload bytes to transmit.
     * @param length Number of bytes to transmit.
     * @return true if the payload is accepted and TX mode is started.
     * @return false if @p data is null or @p length is zero.
     *
     * @note This method does not wait for transmission completion. Call
     *       waitPacketSent() after send() to block until TX_DONE is reported.
     */
    bool send(const uint8_t* data, uint8_t length);

    /**
     * @brief Blocks until the current packet transmission is complete.
     *
     * This method polls the IRQ flags until TX_DONE is set, clears the TX_DONE
     * flag, and returns the radio to standby mode.
     *
     * @warning This call blocks indefinitely if the radio never reports TX_DONE.
     */
    void waitPacketSent();

    /**
     * @brief Checks whether a valid packet is available in the receive FIFO.
     *
     * If no RX_DONE flag is present, the radio is put into continuous receive
     * mode and false is returned. If a CRC error is detected, IRQ flags are
     * cleared, RX continuous mode is restored, and false is returned.
     *
     * @return true when a packet has been received and is ready to read.
     * @return false when no packet is available or a CRC error occurred.
     */
    bool available();

    /**
     * @brief Reads the received packet from the radio FIFO.
     *
     * @param buffer Destination buffer that will receive the packet bytes.
     * @param length Input/output packet length. On input, *length must contain
     *        the capacity of @p buffer. On success, *length is replaced with the
     *        actual number of bytes read.
     * @return true if a packet is available and copied into @p buffer.
     * @return false if arguments are invalid, no packet is available, or the
     *         provided buffer is too small.
     */
    bool recv(uint8_t* buffer, uint8_t* length);

private:
    /** @brief SPI chip-select pin for the LoRa module. */
    uint8_t _csPin;

    /** @brief Interrupt/DIO0 pin for RX/TX IRQ signalling. */
    uint8_t _irqPin;

    /** @brief Hardware reset pin for the LoRa module. */
    uint8_t _resetPin;

    /**
     * @brief Internal packet-availability state placeholder.
     *
     * @note The current implementation determines availability directly from
     *       radio IRQ flags rather than this member.
     */
    bool _packetAvailable;

    /**
     * @brief Performs the LoRa module hardware reset sequence.
     */
    void reset();

    /**
     * @brief Reads one register from the LoRa radio over SPI.
     *
     * @param reg Register address to read.
     * @return Register value returned by the radio.
     */
    uint8_t readRegister(uint8_t reg);

    /**
     * @brief Writes one register in the LoRa radio over SPI.
     *
     * @param reg Register address to write.
     * @param value Value to write into the register.
     */
    void writeRegister(uint8_t reg, uint8_t value);

    /**
     * @brief Reads multiple bytes from a radio FIFO/register address.
     *
     * @param reg Register address used as the read source.
     * @param buffer Destination buffer.
     * @param length Number of bytes to read.
     */
    void readBuffer(uint8_t reg, uint8_t* buffer, uint8_t length);

    /**
     * @brief Writes multiple bytes to a radio FIFO/register address.
     *
     * @param reg Register address used as the write destination.
     * @param buffer Source buffer.
     * @param length Number of bytes to write.
     */
    void writeBuffer(uint8_t reg, const uint8_t* buffer, uint8_t length);

    /** @brief Switches the radio to LoRa sleep mode. */
    void setModeSleep();

    /** @brief Switches the radio to LoRa standby mode. */
    void setModeStandby();

    /** @brief Switches the radio to LoRa transmit mode. */
    void setModeTx();

    /** @brief Switches the radio to LoRa continuous receive mode. */
    void setModeRxContinuous();

    /**
     * @brief Clears all radio IRQ flags.
     */
    void clearIrqFlags();
};

}