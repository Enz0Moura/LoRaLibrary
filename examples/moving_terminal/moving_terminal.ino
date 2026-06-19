#include <SPI.h>
#include <RH_RF95.h>
#include <LoRaLibrary.hpp>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

enum State {
    IDLE,
    INITIALIZING,
    WAITING_FOR_MESSAGE,
    WAITING_FOR_RECORD,
    SENDING_BEACON,
    WAITING_ACK,
    WAITING_BEACON
};

RH_RF95 rf95(RFM95_CS, RFM95_INT);

LocINO::LoRaProtocol lora(rf95, RFM95_RST, RF95_FREQ);
LocINO::SerialProtocol cpu(Serial);

State currentState = INITIALIZING;

bool readBytesFromCpu(
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
    if (Serial.available()) {
        char command = Serial.read();
        handleCommand(command);
    }
}

void handleCommand(char command) {
    if (command == 'M') {
        currentState = WAITING_FOR_MESSAGE;
        waitForMessage();
    } 
    else if (command == 'R') {
        currentState = WAITING_FOR_RECORD;
        listenForRecord();
    } 
    else if (command == 'B') {
        currentState = SENDING_BEACON;
        sendBeacon();
    } 
    else if (command == 'L') {
        currentState = WAITING_BEACON;
        listenForBeacon();
    } 
    else {
        cpu.sendError();
    }
}

void waitForMessage() {
    LocINO::Packet packet;

    if (!readBytesFromCpu(packet, LocINO::MESSAGE_LENGTH, 10000)) {
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

void sendBeacon() {
    LocINO::Packet beacon;

    if (!readBytesFromCpu(beacon, LocINO::BEACON_LENGTH, 10000)) {
        cpu.sendLoRaTimeout();
        currentState = WAITING_FOR_RECORD;
        return;
    }

    unsigned long startTime = millis();

    while (millis() - startTime < 10000) {
        if (!lora.send(beacon)) {
            cpu.sendLoRaTxFail();
            currentState = IDLE;
            return;
        }

        LocINO::Packet response;

        LocINO::LoRaStatus status = lora.receive(
            response,
            1000,
            LocINO::ReceiveMode::AckOnly
        );

        if (status == LocINO::LoRaStatus::Ok) {
            cpu.sendAck();
            currentState = IDLE;
            return;
        }
    }

    cpu.sendLoRaTimeout();
    currentState = IDLE;
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
        currentState = WAITING_FOR_MESSAGE;
    } 
    else if (status == LocINO::LoRaStatus::Timeout) {
        cpu.sendLoRaTimeout();
        currentState = WAITING_FOR_MESSAGE;
    } 
    else {
        cpu.sendError();
        currentState = WAITING_FOR_MESSAGE;
    }
}

void listenForBeacon() {
    LocINO::Packet packet;

    LocINO::LoRaStatus status = lora.receive(
        packet,
        10000,
        LocINO::ReceiveMode::Any
    );

    if (status == LocINO::LoRaStatus::Ok) {
        cpu.sendLoRaReceived(packet);

        LocINO::Packet ack = LocINO::createAck();

        if (lora.send(ack)) {
            cpu.sendAck();
        } else {
            cpu.sendLoRaTxFail();
        }

        currentState = WAITING_FOR_MESSAGE;
    } 
    else if (status == LocINO::LoRaStatus::Timeout) {
        cpu.sendLoRaTimeout();
        currentState = WAITING_FOR_MESSAGE;
    } 
    else {
        cpu.sendError();
        currentState = WAITING_FOR_MESSAGE;
    }
}