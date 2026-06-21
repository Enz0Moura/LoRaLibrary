#include <LocINO/LocINOSerialClient.hpp>
#include <LocINO/LocINOCLI.hpp>

#include <iostream>

int main() {
    LocINO::LocINOSerialClient client("/dev/ttyACM0", 9600);

    LocINO::EventPacket event;

    auto packet = LocINO::LoRaPacket::fromText("Hello World!");

    if (!client.sendMessage(packet)) {
        std::cerr << "Failed to send command\n";
        return 1;
    }

    while (client.receiveEvent(event)) {
        LocINO::CLI::printEvent(event);

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