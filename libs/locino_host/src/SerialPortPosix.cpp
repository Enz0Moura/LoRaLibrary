#include "LocINO/SerialPort.hpp"

#ifndef _WIN32
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>

namespace LocINO {

SerialPort::SerialPort(const std::string& device, int baudRate) {
    _fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

    if (_fd >= 0) {
        configure(baudRate);
    }
}

SerialPort::~SerialPort() {
    if (_fd >= 0) {
        close(_fd);
    }
}

bool SerialPort::configure(int baudRate) {
    termios tty{};

    if (tcgetattr(_fd, &tty) != 0) {
        return false;
    }

    speed_t speed = B9600;

    if (baudRate == 115200) {
        speed = B115200;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_lflag = 0;
    tty.c_oflag = 0;

    // timeout será controlado por poll()
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    return tcsetattr(_fd, TCSANOW, &tty) == 0;
}

bool SerialPort::writeBytes(const uint8_t* data, size_t length) {
    return _fd >= 0 &&
           write(_fd, data, length) == static_cast<ssize_t>(length);
}

bool SerialPort::readByte(uint8_t& byte) {
    return readByte(byte, 1000);
}

bool SerialPort::readByte(uint8_t& byte, unsigned long timeoutMs) {
    if (_fd < 0) {
        return false;
    }

    pollfd pfd{};
    pfd.fd = _fd;
    pfd.events = POLLIN;

    int result = poll(&pfd, 1, static_cast<int>(timeoutMs));

    if (result <= 0) {
        return false;
    }

    return read(_fd, &byte, 1) == 1;
}

bool SerialPort::readBytes(uint8_t* data, size_t length) {
    return readBytes(data, length, 1000);
}

bool SerialPort::readBytes(
    uint8_t* data,
    size_t length,
    unsigned long timeoutMs
) {
    size_t total = 0;

    while (total < length) {
        uint8_t byte = 0;

        if (!readByte(byte, timeoutMs)) {
            return false;
        }

        data[total++] = byte;
    }

    return true;
}

}

#endif