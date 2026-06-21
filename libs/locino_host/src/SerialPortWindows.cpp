#include "LocINO/SerialPort.hpp"

#ifdef _WIN32

namespace LocINO {

static DWORD toWindowsBaudRate(int baudRate) {
    switch (baudRate) {
        case 9600:
            return CBR_9600;
        case 19200:
            return CBR_19200;
        case 38400:
            return CBR_38400;
        case 57600:
            return CBR_57600;
        case 115200:
            return CBR_115200;
        default:
            return CBR_9600;
    }
}

static std::string normalizeDeviceName(const std::string& device) {
    if (device.rfind("\\\\.\\", 0) == 0) {
        return device;
    }

    if (device.rfind("COM", 0) == 0) {
        return "\\\\.\\" + device;
    }

    return device;
}

SerialPort::SerialPort(const std::string& device, int baudRate) {
    std::string normalizedDevice = normalizeDeviceName(device);

    _handle = CreateFileA(
        normalizedDevice.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    if (_handle != INVALID_HANDLE_VALUE) {
        configure(baudRate);
    }
}

SerialPort::~SerialPort() {
    if (_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(_handle);
    }
}

bool SerialPort::configure(int baudRate) {
    DCB dcb{};
    dcb.DCBlength = sizeof(dcb);

    if (!GetCommState(_handle, &dcb)) {
        return false;
    }

    dcb.BaudRate = toWindowsBaudRate(baudRate);
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    dcb.fBinary = TRUE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;

    if (!SetCommState(_handle, &dcb)) {
        return false;
    }

    COMMTIMEOUTS timeouts{};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 1000;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 1000;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(_handle, &timeouts)) {
        return false;
    }

    //PurgeComm(_handle, PURGE_RXCLEAR | PURGE_TXCLEAR);

    return true;
}

bool SerialPort::writeBytes(const uint8_t* data, size_t length) {
    if (_handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD written = 0;

    BOOL ok = WriteFile(
        _handle,
        data,
        static_cast<DWORD>(length),
        &written,
        nullptr
    );

    return ok && written == length;
}

bool SerialPort::readByte(uint8_t& byte) {
    return readBytes(&byte, 1);
}

bool SerialPort::readBytes(uint8_t* data, size_t length) {
    if (_handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    size_t total = 0;

    while (total < length) {
        DWORD bytesRead = 0;

        BOOL ok = ReadFile(
            _handle,
            data + total,
            static_cast<DWORD>(length - total),
            &bytesRead,
            nullptr
        );

        if (!ok || bytesRead == 0) {
            return false;
        }

        total += bytesRead;
    }

    return true;
}

}

#endif