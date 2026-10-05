// include/Logger.h
#pragma once

#include <mutex>
#include <string>

class Logger {
public:
    Logger() = default;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(const std::string& line);

private:
    std::mutex mutex_;
};