#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace LocINO {

class SerialPort {
public:
    SerialPort(const std::string& device, int baudRate);
    ~SerialPort();

    bool writeBytes(const uint8_t* data, size_t length);
    bool readByte(uint8_t& byte);
    bool readBytes(uint8_t* data, size_t length);

private:
    int _fd = -1;

    bool configure(int baudRate);
};

}