#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace LocINO {

/**
 * @brief Cross-platform blocking serial port wrapper.
 *
 * SerialPort hides the platform-specific details required to open, configure,
 * read from, and write to a serial device on POSIX and Windows systems.
 */
class SerialPort {
public:
    /**
     * @brief Opens and configures a serial device.
     *
     * @param device Platform-specific serial device path or port name.
     * @param baudRate Baud rate used to configure the serial connection.
     */
    SerialPort(const std::string& device, int baudRate);

    /**
     * @brief Closes the serial device and releases platform resources.
     */
    ~SerialPort();

    /**
     * @brief Writes a sequence of bytes to the serial device.
     *
     * @param data Pointer to the bytes to write.
     * @param length Number of bytes to write from @p data.
     * @return true if all bytes were written successfully.
     * @return false if the write operation fails or writes fewer bytes than requested.
     */
    bool writeBytes(const uint8_t* data, size_t length);

    /**
     * @brief Reads one byte from the serial device using the default blocking behavior.
     *
     * @param byte Receives the byte read from the serial device.
     * @return true if one byte was read successfully.
     * @return false if the read operation fails.
     */
    bool readByte(uint8_t& byte);

    /**
     * @brief Reads one byte from the serial device with a timeout.
     *
     * @param byte Receives the byte read from the serial device.
     * @param timeoutMs Maximum time, in milliseconds, to wait for one byte.
     * @return true if one byte was read before the timeout.
     * @return false if the timeout expires or the read operation fails.
     */
    bool readByte(uint8_t& byte, unsigned long timeoutMs);

    /**
     * @brief Reads an exact number of bytes using the default blocking behavior.
     *
     * @param data Destination buffer for the received bytes.
     * @param length Number of bytes to read into @p data.
     * @return true if the requested number of bytes was read successfully.
     * @return false if the read operation fails before all bytes are received.
     */
    bool readBytes(uint8_t* data, size_t length);

    /**
     * @brief Reads an exact number of bytes with a timeout.
     *
     * @param data Destination buffer for the received bytes.
     * @param length Number of bytes to read into @p data.
     * @param timeoutMs Maximum time, in milliseconds, to complete the read.
     * @return true if all requested bytes are read before the timeout.
     * @return false if the timeout expires or the read operation fails.
     */
    bool readBytes(uint8_t* data, size_t length, unsigned long timeoutMs);

private:
#ifdef _WIN32
    HANDLE _handle = INVALID_HANDLE_VALUE;
#else
    int _fd = -1;
#endif

    /**
     * @brief Applies platform-specific baud rate and serial mode settings.
     *
     * @param baudRate Baud rate to apply to the opened serial device.
     * @return true if configuration succeeds.
     * @return false if the requested settings cannot be applied.
     */
    bool configure(int baudRate);
};

}