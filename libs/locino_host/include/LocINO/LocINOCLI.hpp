#pragma once
#include <LocINO/Types.hpp>
#include <iomanip>
#include <iostream>

namespace LocINO {
    class CLI {
        public:
        static void printEvent(const LocINO::EventPacket& event);

        private:
            static bool isPrintablePayload(const LocINO::LoRaPacket& packet);
    };
}