#include <LocINO/LocINOSerialClient.hpp>

#include <iomanip>
#include <iostream>

void printEvent(const LocINO::EventPacket& event) {
    using LocINO::CpuEventType;

    switch (event.type) {
        case CpuEventType::LoRaTxOk:
            std::cout << "LoRa TX OK\n";
            break;

        case CpuEventType::LoRaTxFail:
            std::cout << "LoRa TX FAIL\n";
            break;

        case CpuEventType::LoRaTimeout:
            std::cout << "LoRa TIMEOUT\n";
            break;

        case CpuEventType::Ack:
            std::cout << "ACK\n";
            break;

        case CpuEventType::Error:
            std::cout << "ERROR\n";
            break;

        case CpuEventType::LoRaRx:
            std::cout << "LoRa RX: ";

            for (uint8_t i = 0; i < event.length; i++) {
                std::cout << std::hex
                          << std::setw(2)
                          << std::setfill('0')
                          << static_cast<int>(event.data[i])
                          << " ";
            }

            std::cout << std::dec << "\n";
            break;

        default:
            std::cout << "Unknown event\n";
            break;
    }
}

int main() {
    LocINO::LocINOSerialClient client("/dev/ttyACM0", 9600);

    LocINO::EventPacket event;

    auto packet = LocINO::LoRaPacket::fromText("Hello World!");

    if (!client.sendMessage(packet)) {
        std::cerr << "Failed to send command\n";
        return 1;
    }

    while (client.receiveEvent(event)) {
        printEvent(event);

        if (
            event.type == LocINO::CpuEventType::LoRaTxOk ||
            event.type == LocINO::CpuEventType::LoRaTxFail ||
            event.type == LocINO::CpuEventType::LoRaTimeout ||
            event.type == LocINO::CpuEventType::Error
        ) {
            break;
        }
    }

    return 0;
}