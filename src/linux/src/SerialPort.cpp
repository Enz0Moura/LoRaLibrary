#include "SerialPort.hpp"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

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

    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 10;

    return tcsetattr(_fd, TCSANOW, &tty) == 0;
}

bool SerialPort::writeBytes(const uint8_t* data, size_t length) {
    return _fd >= 0 &&
           write(_fd, data, length) == static_cast<ssize_t>(length);
}

bool SerialPort::readByte(uint8_t& byte) {
    return _fd >= 0 && read(_fd, &byte, 1) == 1;
}

bool SerialPort::readBytes(uint8_t* data, size_t length) {
    size_t total = 0;

    while (total < length) {
        ssize_t n = read(_fd, data + total, length - total);

        if (n <= 0) {
            return false;
        }

        total += static_cast<size_t>(n);
    }

    return true;
}

}