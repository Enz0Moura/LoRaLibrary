#include <SPI.h>
#include <RH_RF95.h>
#include <LoRaLibrary.hpp>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

enum State {
    INITIALIZING,
    WAITING_FOR_MESSAGE,
    WAITING_FOR_RECORD,
    IDLE
};

RH_RF95 rf95(RFM95_CS, RFM95_INT);

LocINO::LoRaProtocol lora(rf95, RFM95_RST, RF95_FREQ);
LocINO::SerialProtocol cpu(Serial);

State currentState = INITIALIZING;

bool readPacketFromCpu(
    LocINO::Packet& packet,
    uint8_t expectedLength,
    unsigned long timeoutMs
);

void setup() {
    cpu.begin(9600);

    while (!Serial);
    delay(100);

    if (!lora.begin()) {
        cpu.sendError();
        while (1);
    }

    currentState = WAITING_FOR_MESSAGE;
    cpu.sendAck();
}

void loop() {
    switch (currentState) {
        case WAITING_FOR_MESSAGE:
            waitForMessage();
            break;

        case WAITING_FOR_RECORD:
            listenForRecord();
            break;

        case IDLE:
            currentState = WAITING_FOR_MESSAGE;
            break;

        default:
            currentState = WAITING_FOR_MESSAGE;
            break;
    }
}

void waitForMessage() {
    LocINO::Packet packet;

    if (!readPacketFromCpu(packet, LocINO::MESSAGE_LENGTH, 10000)) {
        cpu.sendLoRaTimeout();
        currentState = WAITING_FOR_RECORD;
        return;
    }

    if (lora.send(packet)) {
        cpu.sendLoRaTxOk();
        currentState = IDLE;
    } else {
        cpu.sendLoRaTxFail();
        currentState = WAITING_FOR_MESSAGE;
    }
}

void listenForRecord() {
    LocINO::Packet packet;

    LocINO::LoRaStatus status = lora.receive(
        packet,
        10000,
        LocINO::ReceiveMode::Any
    );

    if (status == LocINO::LoRaStatus::Ok) {
        cpu.sendLoRaReceived(packet);
    } 
    else if (status == LocINO::LoRaStatus::Timeout) {
        cpu.sendLoRaTimeout();
    } 
    else {
        cpu.sendError();
    }

    currentState = WAITING_FOR_MESSAGE;
}

bool readPacketFromCpu(
    LocINO::Packet& packet,
    uint8_t expectedLength,
    unsigned long timeoutMs
) {
    unsigned long startTime = millis();

    while (Serial.available() < expectedLength) {
        if (millis() - startTime > timeoutMs) {
            packet.length = 0;
            return false;
        }
    }

    packet.length = expectedLength;
    Serial.readBytes(packet.data, expectedLength);

    return true;
}