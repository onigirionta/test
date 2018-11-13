#pragma once

#include "input.h"

#include <windows.h>

#include <stdexcept>
#include <string>

class CommPort : public Source {
public:
    explicit CommPort(const std::string& path);

    CommPort(const CommPort&) = delete;
    CommPort& operator=(const CommPort&) = delete;

    ~CommPort() override;

    size_t read(void* buffer, size_t size) override;

private:
    HANDLE _handle{INVALID_HANDLE_VALUE};
};

// Конструктор класса CommPort
CommPort::CommPort(const std::string& path) {
    //Открываем первый com для чтения или записи
    _handle = ::CreateFile(
            path.c_str(),
            GENERIC_READ /*| GENERIC_WRITE*/,
            0,
            0,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0);

    if (_handle == INVALID_HANDLE_VALUE) {
        DWORD error_code = GetLastError();
        throw std::runtime_error("Unable to access port, error code " + std::to_string(error_code));
    }

    //настройка параметров соединения:
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams); //раземер структуры

    if (!::GetCommState(_handle, &dcbSerialParams)) {
        DWORD error_code = GetLastError();
        throw std::runtime_error("Getting state error, error code " + std::to_string(error_code));
    }
    dcbSerialParams.BaudRate = 460800;      //скорость потока
    dcbSerialParams.ByteSize = 8;           //определяет число информационных бит в передаваемых и принимаемых байтах
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;      //отсутствие бита четности

    if (!SetCommState(_handle, &dcbSerialParams)) {
        DWORD error_code = GetLastError();
        throw std::runtime_error("Error setting serial port state, error code " + std::to_string(error_code));
    }
}

// деструктор CommPort
CommPort::~CommPort() {
    ::CloseHandle(_handle);
}

//метод read для работы с классом CommPort
size_t
CommPort::read(void* buffer, size_t size) {
    DWORD bytes_read{};
    const auto result = ::ReadFile(_handle, buffer, size, &bytes_read, nullptr);
    if (!result) {
        const auto error_code = GetLastError();
        if (error_code == ERROR_OPERATION_ABORTED) {
            throw EOFError{};
        }
        throw std::runtime_error("Unable to read data from ComPort, error code " + std::to_string(error_code));
    }
    return bytes_read;
}
