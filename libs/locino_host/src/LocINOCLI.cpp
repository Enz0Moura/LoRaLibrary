#include <LocINO/LocINOCLI.hpp>

namespace LocINO{
    void CLI::printEvent(const LocINO::EventPacket& event) {
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
}