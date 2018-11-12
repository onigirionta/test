#pragma once

#include "input.h"

#include <iostream>

class Stream : public Source {
public:
    explicit Stream(std::istream& stream) : _stream{stream} {}

    size_t read(void* buffer, size_t size) override {
        // FIXME: very slow, for testing only
        const auto bytes_read = _stream.readsome(reinterpret_cast<char*>(buffer), size);
        if (_stream.eof()) {
            throw EOFError{};
        }
        if (!_stream) {
            throw std::runtime_error{"stream read error"};
        }
        return bytes_read;
    }

private:
    std::istream& _stream;
};
