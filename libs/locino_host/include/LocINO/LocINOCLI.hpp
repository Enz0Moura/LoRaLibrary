#pragma once
#include <LocINO/Types.hpp>
#include <iomanip>
#include <iostream>

namespace LocINO {
    /**
     * @brief Utility class for printing LocINO protocol events to standard output.
     *
     * CLI centralizes host-side formatting for received controller events so that
     * command-line examples and tools present packets consistently.
     */
    class CLI {
        public:
        /**
         * @brief Prints a decoded LocINO event in a human-readable format.
         *
         * @param event Event packet received from a LocINO serial client.
         */
        static void printEvent(const LocINO::EventPacket& event);

        private:
            /**
             * @brief Determines whether a LoRa packet payload can be printed as text.
             *
             * @param packet Packet whose payload bytes should be inspected.
             * @return true if the payload appears to contain printable characters.
             * @return false if the payload should be displayed as raw bytes.
             */
            static bool isPrintablePayload(const LocINO::LoRaPacket& packet);
    };
}