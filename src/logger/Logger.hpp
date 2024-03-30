#pragma once

#include "sead/heap/seadDisposer.h"
#include "nn/util.h"

enum class LoggerState {
    UNINITIALIZED = 0,
    CONNECTED = 1,
    UNAVAILABLE = 2,
    DISCONNECTED = 3
};

class Logger {
    public:
        Logger() = default;

        static Logger& instance();

        bool init(const char* ip, u16 port);

        static void log(const char *fmt, ...);
        static void log(const char *fmt, va_list args);
        static bool stringToIPAddress(const char* str, struct in_addr* out);

    private:
        LoggerState mState;
        int mSocketFd;
};
