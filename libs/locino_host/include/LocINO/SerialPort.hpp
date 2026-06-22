#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace LocINO {

class SerialPort {
public:
    SerialPort(const std::string& device, int baudRate);
    ~SerialPort();

    bool writeBytes(const uint8_t* data, size_t length);

    bool readByte(uint8_t& byte);
    bool readByte(uint8_t& byte, unsigned long timeoutMs);

    bool readBytes(uint8_t* data, size_t length);
    bool readBytes(uint8_t* data, size_t length, unsigned long timeoutMs);

private:
#ifdef _WIN32
    HANDLE _handle = INVALID_HANDLE_VALUE;
#else
    int _fd = -1;
#endif

    bool configure(int baudRate);
};

}