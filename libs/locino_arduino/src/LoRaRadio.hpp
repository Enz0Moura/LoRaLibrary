#pragma once

#include <Arduino.h>
#include <SPI.h>

namespace LocINO {

class LoRaRadio {
public:
    LoRaRadio(uint8_t csPin, uint8_t irqPin, uint8_t resetPin);

    bool init();
    bool setFrequency(float frequencyMHz);
    void setTxPower(uint8_t powerDbm);
    void setModemConfigBw125Cr45Sf128();

    bool send(const uint8_t* data, uint8_t length);
    void waitPacketSent();

    bool available();
    bool recv(uint8_t* buffer, uint8_t* length);

private:
    uint8_t _csPin;
    uint8_t _irqPin;
    uint8_t _resetPin;

    bool _packetAvailable;

    void reset();

    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);

    void readBuffer(uint8_t reg, uint8_t* buffer, uint8_t length);
    void writeBuffer(uint8_t reg, const uint8_t* buffer, uint8_t length);

    void setModeSleep();
    void setModeStandby();
    void setModeTx();
    void setModeRxContinuous();

    void clearIrqFlags();
};

}