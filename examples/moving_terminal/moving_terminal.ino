
#include <SPI.h>

#include <LoRaLibrary.hpp>
#include <LoRaRadio.hpp>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

LocINO::LoRaRadio radio(RFM95_CS, RFM95_INT, RFM95_RST);

LocINO::LoRaProtocol lora(radio, RFM95_RST, RF95_FREQ);
LocINO::SerialProtocol cpu(Serial);

void setup() {
    cpu.begin(9600);

    while (!Serial);
    delay(100);

    if (!lora.begin()) {
        cpu.sendError();
        while (1);
    }

    cpu.sendAck();
}

void loop() {
    LocINO::CommandPacket command;

    if (!cpu.receiveCommand(command)) {
        return;
    }

    handleCommand(command);
}

void handleCommand(const LocINO::CommandPacket& command) {
    switch (command.type) {
        case LocINO::CpuCommandType::LoRaSendMessage:
            handleSendMessage(command.toLoRaPacket());
            break;

        case LocINO::CpuCommandType::LoRaSendBeacon:
            handleSendBeacon(command.toLoRaPacket());
            break;

        case LocINO::CpuCommandType::LoRaListenRecord:
            handleListenRecord();
            break;

        case LocINO::CpuCommandType::LoRaListenBeacon:
            handleListenBeacon();
            break;

        default:
            cpu.sendError();
            break;
    }
}

void handleSendMessage(const LocINO::LoRaPacket& packet) {
    if (lora.send(packet)) {
        cpu.sendLoRaTxOk();
    } else {
        cpu.sendLoRaTxFail();
    }
}

void handleSendBeacon(const LocINO::LoRaPacket& packet) {
    LocINO::LoRaStatus status = lora.sendBeacon(packet);

    if (status == LocINO::LoRaStatus::Ok) {
        cpu.sendAck();
    } else if (status == LocINO::LoRaStatus::Timeout) {
        cpu.sendLoRaTimeout();
    } else {
        cpu.sendLoRaTxFail();
    }
}

void handleListenRecord() {
    LocINO::LoRaPacket packet;

    LocINO::LoRaStatus status = lora.receive(packet);

    if (status == LocINO::LoRaStatus::Ok) {
        cpu.sendLoRaReceived(packet);
    } else if (status == LocINO::LoRaStatus::Timeout) {
        cpu.sendLoRaTimeout();
    } else {
        cpu.sendError();
    }
}

void handleListenBeacon() {
    LocINO::LoRaPacket packet;

    LocINO::LoRaStatus status = lora.receive(packet);

    if (status != LocINO::LoRaStatus::Ok) {
        if (status == LocINO::LoRaStatus::Timeout) {
            cpu.sendLoRaTimeout();
        } else {
            cpu.sendError();
        }

        return;
    }

    cpu.sendLoRaReceived(packet);

    LocINO::LoRaPacket ack = LocINO::createAck();

    if (lora.send(ack)) {
        cpu.sendAck();
    } else {
        cpu.sendLoRaTxFail();
    }
}