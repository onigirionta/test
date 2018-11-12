#pragma once

#include <stdexcept>
#include <string>

class EOFError : public std::runtime_error {
public:
    EOFError() : std::runtime_error{"end of file"} {}
};

//абстрактный класс-родитель для различных источников ввода данных
class Source {
public:
    virtual ~Source() = default;

    virtual size_t read(void *buffer, size_t size) = 0;
};

//считывание строчки из буфера
std::string
read_line(Source &source) {
    std::string current_str{};
    current_str.reserve(512);

    char c{};
    while (c != '\n') {
        source.read(&c, sizeof c);
        current_str += c;
    }
    return current_str;
}
