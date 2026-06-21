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
            {
                std::cout << "LoRa RX: ";

                for (uint8_t i = 0; i < event.length; i++) {
                    std::cout << std::hex
                            << std::setw(2)
                            << std::setfill('0')
                            << static_cast<int>(event.data[i])
                            << " ";
                }

                std::cout << std::dec;

                LocINO::LoRaPacket packet;
                packet.length = event.length;
                memcpy(packet.data, event.data, event.length);

                if (CLI::isPrintablePayload(packet)) {
                    std::cout << " | Text: \"";

                    for (uint8_t i = LocINO::HEADER_SIZE;
                        i < packet.length;
                        ++i) {
                        std::cout << static_cast<char>(packet.data[i]);
                    }

                    std::cout << "\"";
                }

                std::cout << "\n";
                break;
            }

            default:
                std::cout << "Unknown event\n";
                break;
        }
    }

    bool CLI::isPrintablePayload(const LocINO::LoRaPacket& packet) {
        if (packet.length <= LocINO::HEADER_SIZE) {
            return false;
        }

        for (uint8_t i = LocINO::HEADER_SIZE; i < packet.length; ++i) {
            if (!isprint(packet.data[i]) &&
                packet.data[i] != '\r' &&
                packet.data[i] != '\n' &&
                packet.data[i] != '\t') {
                return false;
            }
        }

        return true;
    }
}