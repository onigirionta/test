#pragma once

#include "input.h"

#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>

//ложный источник для скармливания тестового потока из текстового файла
class File : public Source {
public:
    explicit File(const std::string& path) {
        _file = std::fopen(path.c_str(), "rb");
        if (!_file) {
            throw std::runtime_error("unable open file '" + path + "'");
        }
    }

    File(const File&) = delete;
    File& operator=(const File&) = delete;

    ~File() override {
        std::fclose(_file);
    }

    size_t
    read(void* buffer, size_t size) override {
        const auto bytes_read = std::fread(buffer, size, 1, _file);
        if (!bytes_read) {
            const auto error_code = errno;
            if (std::feof(_file)) {
                throw EOFError{};
            }
            throw std::runtime_error{std::string{"file read error: "} + strerror(error_code)};
        }
        return bytes_read;
    }

private:
    FILE* _file;
};
