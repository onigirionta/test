#pragma once

#include <windows.h>

#include <system_error>

class Stoppable {
public:
    virtual void stop() = 0;
};

class Terminator {
public:
    static void setup(Stoppable& stoppable);

private:
    static BOOL WINAPI handle_signal(DWORD);
    static VOID CALLBACK handle_event(PVOID, BOOLEAN);

    static Stoppable* _stoppable;
};

Stoppable* Terminator::_stoppable{};

void
Terminator::setup(Stoppable& stoppable) {
    _stoppable = &stoppable;

    if (!::SetConsoleCtrlHandler(handle_signal, TRUE)) {
        throw std::system_error(::GetLastError(), std::system_category());
    }

    constexpr bool manual_reset = false;
    constexpr bool initial_state = false;
    const auto event = ::CreateEvent(nullptr, manual_reset, initial_state, "Startrap_StopRequested");
    if (event == INVALID_HANDLE_VALUE) {
        const auto error = ::GetLastError();
        if (error != ERROR_ALREADY_EXISTS) {
            throw std::system_error(error, std::system_category());
        }
    }

    HANDLE wait_object{INVALID_HANDLE_VALUE};
    if (!::RegisterWaitForSingleObject(&wait_object, event, handle_event, nullptr, INFINITE, 0)) {
        ::CloseHandle(event);
        throw std::system_error(::GetLastError(), std::system_category());
    }
}

BOOL WINAPI
Terminator::handle_signal(DWORD) {
    _stoppable->stop();
    return TRUE;
}

VOID CALLBACK
Terminator::handle_event(PVOID, BOOLEAN) {
    _stoppable->stop();
}
