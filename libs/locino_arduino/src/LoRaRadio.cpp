#include "LoRaRadio.hpp"

namespace LocINO {

static constexpr uint8_t REG_FIFO = 0x00;
static constexpr uint8_t REG_OP_MODE = 0x01;
static constexpr uint8_t REG_FRF_MSB = 0x06;
static constexpr uint8_t REG_FRF_MID = 0x07;
static constexpr uint8_t REG_FRF_LSB = 0x08;
static constexpr uint8_t REG_PA_CONFIG = 0x09;
static constexpr uint8_t REG_LNA = 0x0C;
static constexpr uint8_t REG_FIFO_ADDR_PTR = 0x0D;
static constexpr uint8_t REG_FIFO_TX_BASE_ADDR = 0x0E;
static constexpr uint8_t REG_FIFO_RX_BASE_ADDR = 0x0F;
static constexpr uint8_t REG_FIFO_RX_CURRENT_ADDR = 0x10;
static constexpr uint8_t REG_IRQ_FLAGS = 0x12;
static constexpr uint8_t REG_RX_NB_BYTES = 0x13;
static constexpr uint8_t REG_PKT_SNR_VALUE = 0x19;
static constexpr uint8_t REG_PKT_RSSI_VALUE = 0x1A;
static constexpr uint8_t REG_MODEM_CONFIG_1 = 0x1D;
static constexpr uint8_t REG_MODEM_CONFIG_2 = 0x1E;
static constexpr uint8_t REG_PREAMBLE_MSB = 0x20;
static constexpr uint8_t REG_PREAMBLE_LSB = 0x21;
static constexpr uint8_t REG_PAYLOAD_LENGTH = 0x22;
static constexpr uint8_t REG_MODEM_CONFIG_3 = 0x26;
static constexpr uint8_t REG_DETECTION_OPTIMIZE = 0x31;
static constexpr uint8_t REG_DETECTION_THRESHOLD = 0x37;
static constexpr uint8_t REG_SYNC_WORD = 0x39;
static constexpr uint8_t REG_VERSION = 0x42;
static constexpr uint8_t REG_PA_DAC = 0x4D;

static constexpr uint8_t MODE_LONG_RANGE_MODE = 0x80;
static constexpr uint8_t MODE_SLEEP = 0x00;
static constexpr uint8_t MODE_STDBY = 0x01;
static constexpr uint8_t MODE_TX = 0x03;
static constexpr uint8_t MODE_RX_CONTINUOUS = 0x05;

static constexpr uint8_t IRQ_RX_DONE = 0x40;
static constexpr uint8_t IRQ_TX_DONE = 0x08;
static constexpr uint8_t IRQ_PAYLOAD_CRC_ERROR = 0x20;

static constexpr uint8_t FIFO_TX_BASE_ADDR = 0x00;
static constexpr uint8_t FIFO_RX_BASE_ADDR = 0x00;

LoRaRadio::LoRaRadio(uint8_t csPin, uint8_t irqPin, uint8_t resetPin)
    : _csPin(csPin),
      _irqPin(irqPin),
      _resetPin(resetPin),
      _packetAvailable(false) {}

bool LoRaRadio::init() {
    pinMode(_csPin, OUTPUT);
    pinMode(_resetPin, OUTPUT);
    pinMode(_irqPin, INPUT);

    digitalWrite(_csPin, HIGH);

    SPI.begin();

    reset();

    uint8_t version = readRegister(REG_VERSION);

    if (version != 0x12) {
        return false;
    }

    setModeSleep();

    writeRegister(REG_FIFO_TX_BASE_ADDR, FIFO_TX_BASE_ADDR);
    writeRegister(REG_FIFO_RX_BASE_ADDR, FIFO_RX_BASE_ADDR);

    writeRegister(REG_LNA, readRegister(REG_LNA) | 0x03);

    setModemConfigBw125Cr45Sf128();

    writeRegister(REG_PREAMBLE_MSB, 0x00);
    writeRegister(REG_PREAMBLE_LSB, 0x08);

    writeRegister(REG_SYNC_WORD, 0x12);

    clearIrqFlags();

    setModeStandby();

    return true;
}

bool LoRaRadio::setFrequency(float frequencyMHz) {
    uint64_t frf = static_cast<uint64_t>((frequencyMHz * 1000000.0) / 61.03515625);

    writeRegister(REG_FRF_MSB, static_cast<uint8_t>((frf >> 16) & 0xFF));
    writeRegister(REG_FRF_MID, static_cast<uint8_t>((frf >> 8) & 0xFF));
    writeRegister(REG_FRF_LSB, static_cast<uint8_t>(frf & 0xFF));

    return true;
}

void LoRaRadio::setTxPower(uint8_t powerDbm) {
    if (powerDbm < 2) {
        powerDbm = 2;
    }

    if (powerDbm > 17) {
        powerDbm = 17;
    }

    writeRegister(REG_PA_CONFIG, 0x80 | (powerDbm - 2));
}

void LoRaRadio::setModemConfigBw125Cr45Sf128() {
    // BW = 125 kHz, CR = 4/5, explicit header mode
    writeRegister(REG_MODEM_CONFIG_1, 0x72);

    // SF7, CRC enabled
    writeRegister(REG_MODEM_CONFIG_2, 0x74);

    // Low data rate optimization disabled, AGC auto enabled
    writeRegister(REG_MODEM_CONFIG_3, 0x04);

    writeRegister(REG_DETECTION_OPTIMIZE, 0x03);
    writeRegister(REG_DETECTION_THRESHOLD, 0x0A);
}

bool LoRaRadio::send(const uint8_t* data, uint8_t length) {
    if (data == nullptr || length == 0) {
        return false;
    }

    if (length > 255) {
        return false;
    }

    setModeStandby();
    clearIrqFlags();

    writeRegister(REG_FIFO_ADDR_PTR, FIFO_TX_BASE_ADDR);
    writeBuffer(REG_FIFO, data, length);
    writeRegister(REG_PAYLOAD_LENGTH, length);

    setModeTx();

    return true;
}

void LoRaRadio::waitPacketSent() {
    while ((readRegister(REG_IRQ_FLAGS) & IRQ_TX_DONE) == 0) {
        yield();
    }

    writeRegister(REG_IRQ_FLAGS, IRQ_TX_DONE);
    setModeStandby();
}

bool LoRaRadio::available() {
    uint8_t irqFlags = readRegister(REG_IRQ_FLAGS);

    if ((irqFlags & IRQ_RX_DONE) == 0) {
        setModeRxContinuous();
        return false;
    }

    if (irqFlags & IRQ_PAYLOAD_CRC_ERROR) {
        clearIrqFlags();
        setModeRxContinuous();
        return false;
    }

    return true;
}

bool LoRaRadio::recv(uint8_t* buffer, uint8_t* length) {
    if (buffer == nullptr || length == nullptr) {
        return false;
    }

    if (!available()) {
        return false;
    }

    uint8_t packetLength = readRegister(REG_RX_NB_BYTES);
    uint8_t currentAddr = readRegister(REG_FIFO_RX_CURRENT_ADDR);

    if (*length < packetLength) {
        clearIrqFlags();
        setModeRxContinuous();
        return false;
    }

    writeRegister(REG_FIFO_ADDR_PTR, currentAddr);
    readBuffer(REG_FIFO, buffer, packetLength);

    *length = packetLength;

    clearIrqFlags();
    setModeRxContinuous();

    return true;
}

void LoRaRadio::reset() {
    digitalWrite(_resetPin, LOW);
    delay(10);
    digitalWrite(_resetPin, HIGH);
    delay(10);
}

uint8_t LoRaRadio::readRegister(uint8_t reg) {
    digitalWrite(_csPin, LOW);

    SPI.transfer(reg & 0x7F);
    uint8_t value = SPI.transfer(0x00);

    digitalWrite(_csPin, HIGH);

    return value;
}

void LoRaRadio::writeRegister(uint8_t reg, uint8_t value) {
    digitalWrite(_csPin, LOW);

    SPI.transfer(reg | 0x80);
    SPI.transfer(value);

    digitalWrite(_csPin, HIGH);
}

void LoRaRadio::readBuffer(uint8_t reg, uint8_t* buffer, uint8_t length) {
    digitalWrite(_csPin, LOW);

    SPI.transfer(reg & 0x7F);

    for (uint8_t i = 0; i < length; i++) {
        buffer[i] = SPI.transfer(0x00);
    }

    digitalWrite(_csPin, HIGH);
}

void LoRaRadio::writeBuffer(uint8_t reg, const uint8_t* buffer, uint8_t length) {
    digitalWrite(_csPin, LOW);

    SPI.transfer(reg | 0x80);

    for (uint8_t i = 0; i < length; i++) {
        SPI.transfer(buffer[i]);
    }

    digitalWrite(_csPin, HIGH);
}

void LoRaRadio::setModeSleep() {
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
    delay(10);
}

void LoRaRadio::setModeStandby() {
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
    delay(10);
}

void LoRaRadio::setModeTx() {
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);
}

void LoRaRadio::setModeRxContinuous() {
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
}

void LoRaRadio::clearIrqFlags() {
    writeRegister(REG_IRQ_FLAGS, 0xFF);
}

}